#include "./include/planner/planner.h"

void freeQueryplan(Operator *node) {

    if (node == NULL) {
        return;
    }

    if (node->child != NULL) {
        freeQueryplan(node->child);
    }


    if (node->type == OP_JOIN) {
        freeQueryplan(node->info.join.left);
        freeQueryplan(node->info.join.right);
    }

    if (node->type == OP_FILTER) {
        freeQueryplan(node->info.filter.next);
    }

    free(node);
}


Operator* makeScanOp(Node* node) {
    
    if (node->type != FILEPATH) {
        printf("Tried to make scan-operator from something else than a FILEPATH. Type: %d\n", node->type);
        exit(1);
    }

    TableMetadata tbl;
    memset(&tbl, 0, sizeof(tbl));
    catalogFile(node->content, &tbl, ';');

    Operator* op = (Operator*) calloc(1, sizeof(Operator));
    op->type = OP_SCAN;
    memcpy(&op->info.scan.table, &tbl, sizeof(tbl));
    op->child                   = NULL;
    op->info.scan.tablefile     = NULL;
    op->info.scan.cursor        = 0;
    op->getTuple                = NULL;

    for (size_t i = 0; i < tbl.columnCount; i++) {
        op->resultDescription.columns[i].type = tbl.columns[i].type;
        op->resultDescription.columns[i].identifier = tbl.columns[i].identifier;
        strcpy(op->resultDescription.columns[i].name, tbl.columns[i].name);

    }
    op->resultDescription.columnCount = tbl.columnCount;

    return op;
}


int mapBoolOpToInt(char* boolOp) {
    if (strcmp(boolOp, "=") == 0) {
        return -1;
    }
    if (strcmp(boolOp, "!=") == 0) {
        return -2;
    }
    if (strcmp(boolOp, "<") == 0) {
        return -3;
    }
    if (strcmp(boolOp, ">") == 0) {
        return -4;
    }
    printf("Don't know how to handle boolean operator %s\n", boolOp);
    exit(1);
}


Node* mapBoolExpr(Node* node, ResultSet* childResultDesc, int* boolExprList, size_t* boolExprListSize) {
    if (node == NULL) {
        printf("Passed NULL-pointer to mapBoolExpr.\n");
        exit(1);
    }
    
    if (node->next == NULL) {
        printf("Making bool expr failed. Node has no next (content: '%s')\n", node->content);
        exit(1);
    }

    if (node->next->next == NULL) {
        printf("Making bool expr failed. Node's next has no next (content: '%s')\n", node->content);
        exit(1);
    }


    Node* value1    = node;
    Node* op        = node->next;
    Node* value2    = node->next->next;

    if (op->type != BOOLOP) {
        printf("Should've been a bool operator, but was %d (content %s)\n", op->type, op->content);
        exit(1);
    }


    int value1Loc = -1;
    int value2Loc = -1;

    for (size_t i = 0; i < childResultDesc->columnCount; i++) {
        if (value1Loc >= 0 && value2Loc >= 0) break;

        if (strcmp(value1->content, childResultDesc->columns[i].name) == 0) {
            value1Loc = i;
        }
        
        if (strcmp(value2->content, childResultDesc->columns[i].name) == 0) {
            value2Loc = i;
        }
    }

    if (value1Loc < 0 && value1->type == IDENT_COL) {
        printf("Could not find column '%s' in result description\n", value1->content);
        exit(1);
    }

    if (value2Loc < 0 && value2->type == IDENT_COL) {
        printf("Could not find column '%s' in result description\n", value2->content);
        exit(1);
    }

    /*
        We will use a crazy decoding for bool exprs.
        Negative numbers are boolean operators:
            -1 equals '='
            -2 equals '!='
            -3 equals '<'
            -4 equals '>'
        
        Positive integers are column indexes in the child result description.

        Therefore, an array of [0,-1,2] means testing equality of the first and third column.
    */


    int boolOp = mapBoolOpToInt(op->content);

    boolExprList[(*boolExprListSize)++] = value1Loc;
    boolExprList[(*boolExprListSize)++] = boolOp;
    boolExprList[(*boolExprListSize)++] = value2Loc;

    return value2->next;

}

void boolExprAddTypes(Node* node, enum nodeType* types) {

    types[0] = node->type;
    types[1] = node->next->type;
    types[2] = node->next->next->type;

}

void boolExprAddConstants(Node* node, char (*charConstants)[FILTERSIZE], int* intConstants) {

    size_t i = 0;
    Node* p_node = node;
    while (i < 3) {
        if (p_node == NULL) {
            printf("Something went wrong with boolExprAddConstants\n");
            exit(1);
        }

        if (p_node->type == STRING || p_node->type == NUMBER)
            strcpy(charConstants[i], p_node->content);
        if (p_node->type == NUMBER) {
            intConstants[i] = atoi(p_node->content);
        }
            
        p_node = p_node->next;
        i++;
    }


}


Operator* makeFilterOp(Node* node, Operator* child) {

    Operator* op = (Operator*) calloc(1, sizeof(Operator));
    op->type = OP_FILTER;
    op->child                   = NULL;
    op->getTuple                = NULL;

    op->info.filter.boolExprListSize = 0;

    memcpy(&op->resultDescription, &child->resultDescription, sizeof(child->resultDescription));


    mapBoolExpr(
        node,
        &child->resultDescription,
        op->info.filter.boolExprList,
        &op->info.filter.boolExprListSize
    );

    boolExprAddTypes(
        node,
        op->info.filter.exprTypes
    );

    boolExprAddConstants(
        node,
        op->info.filter.charConstants,
        op->info.filter.intConstants
    );

    return op;
}


Operator* makeFilterOps(Node* where_node, Operator* child) {

    /*  Returns a linked list of OP_FILTER operators.
        Each have a resultsDescription, but only the first's one is depended on.
    */

    Node* node = where_node->child;
    Operator* op_filt = makeFilterOp(node, child);
    
    /* An boolean expr node is expected to have three linked nodes and inbetween 'AND' or 'OR'. So iterate +4 nodes until NULL-pointer */
    Operator* cur_op = op_filt;
    enum nodeType optype = AND;
    cur_op->info.filter.operatorNext = optype;

    while (node != NULL) {
        // Check the assumption holds
        if (node->next == NULL)                     break;
        if (node->next->next == NULL)               break;
        if (node->next->next->next == NULL)         break;
        if (node->next->next->next->next == NULL)   break;
        optype = node->next->next->next->type;
        if (optype != AND && optype != OR) {
            printf("Unknown boolean operator %d\n", optype);
            exit(1);
        }
        node = node->next->next->next->next;
        cur_op->info.filter.next = makeFilterOp(node, child);
        cur_op->info.filter.operatorNext = optype;
        cur_op = cur_op->info.filter.next;        
    }

    return op_filt;
}
Operator* makeProjectOp(Node* node, Operator* child_op) {



    if (child_op == NULL) {
        printf("Passed a NULL-pointer as child to makeProjectOp\n");
        exit(1);
    }

    Operator* op = (Operator*) calloc(1, sizeof(Operator));
    op->type    = OP_PROJECT;
    op->child   = NULL;

    /*  We know the result set of the child of project op. So we can just which indexes match 
        the projected columns
    */
    int i = 0;

    for (;;) {
        
        op->info.project.colCount++;
        strcpy(op->info.project.columnsToProject[i], node->content);
        op->resultDescription.columns[i].type = node->dtype;
        op->resultDescription.columns[i].identifier = node->identifier;
        strcpy(op->resultDescription.columns[i].name, node->content); 
        i++;
        node = node->next;        
        if (node == NULL) {
            break;
        }
    };

    op->resultDescription.columnCount = op->info.project.colCount;

    bool matched;
    for (i = 0; i < op->info.project.colCount; i++) {
        matched = false;
        for (size_t j = 0; j < child_op->resultDescription.columnCount; j++) {
            if (
                strcmp(op->info.project.columnsToProject[i], child_op->resultDescription.columns[j].name) == 0
            ) {
                op->info.project.colRefs[i] = j;
                op->resultDescription.columns[i].type = child_op->resultDescription.columns[j].type;
                matched = true;
                break;
            }
        }
        if (!matched) {
            printf("Making projection failed: Unable to find column '%s'\n", op->info.project.columnsToProject[i]);
            exit(1);
        }
    }

    return op;
}


Operator* buildFrom(Node* node) {
    /* Node is the first child of FROM-type node */
    if (node->next != NULL && node->next->type == JOIN) {
        Operator* op_join = (Operator*) calloc(1, sizeof(Operator));
        op_join->info.join.left     = makeScanOp(node);
        op_join->info.join.right    = makeScanOp(node->next->next);
        op_join->type = OP_JOIN;
        op_join->info.join.rightTupleCount = 0;
        op_join->info.join.rightTupleIdx = 0;
        op_join->info.join.rightTuplesCollected = false;


        ResultSet result_desc = op_join->info.join.left->resultDescription;
        for (size_t i = 0; i < result_desc.columnCount; i++) {
            op_join->resultDescription.columns[i].type = result_desc.columns[i].type;
            strcpy(op_join->resultDescription.columns[i].name, result_desc.columns[i].name);
        }
        op_join->resultDescription.columnCount = result_desc.columnCount;

        result_desc = op_join->info.join.right->resultDescription;
        size_t rd_i = op_join->resultDescription.columnCount;
        for (size_t i = 0; i < result_desc.columnCount; i++) {
            op_join->resultDescription.columns[i + rd_i].type = result_desc.columns[i].type;
            strcpy(op_join->resultDescription.columns[i + rd_i].name, result_desc.columns[i].name);
        }
        op_join->resultDescription.columnCount += result_desc.columnCount;

        /* ON-clause */
        Node* ON = node->next->next->next;
        Operator* op_filter = makeFilterOps(ON, op_join);

        op_join->info.join.filter = op_filter;

        return op_join;
    }
    if (node->type == FILEPATH) {
        return makeScanOp(node);
    }
    return NULL;
}


Operator* planQuery(Node* nodeSELECT) {


    Operator* op_proj;
    
    Node* FROM = nodeSELECT->next;
    Operator* op_from = NULL;

    if (FROM->child->type == SELECT) {
        op_from = planQuery(FROM->child);
    } else {
        op_from = buildFrom(FROM->child);
    }

    Node* WHERE = nodeSELECT->next->next;

    if (WHERE != NULL) {
        
        Operator* op_filt = makeFilterOps(WHERE, op_from);
        op_proj = makeProjectOp(nodeSELECT->child, op_filt);
        
        op_proj->child  = op_filt;
        op_filt->child  = op_from;

    } else {
        op_proj = makeProjectOp(nodeSELECT->child, op_from);
        op_proj->child = op_from;
    }


    return op_proj;   
}