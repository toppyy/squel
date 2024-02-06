#include "./include/planner/planner.h"

void freeQueryplan(Operator *node) {

    if (node->child != NULL) {
        freeQueryplan(node->child);
    }

    free(node);
}


Operator* makeScanOp(TableMetadata tbl) {

    Operator* op = (Operator*) calloc(1, sizeof(Operator));
    op->type = OP_SCAN;
    memcpy(&op->info.scan.table, &tbl, sizeof(tbl));
    op->next                    = NULL;
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

void boolExprAddConstants(Node* node, char (*constants)[FILTERSIZE]) {

    size_t i = 0;
    Node* p_node = node;
    while (i < 3) {
        if (p_node == NULL) {
            printf("Something went wrong with boolExprAddConstants\n");
            exit(1);
        }

        if (p_node->type == STRING) strcpy(constants[i], p_node->content);
        p_node = p_node->next;
        i++;
    }


}

Operator* makeFilterOp(Node* node, Operator* child) {

    Operator* op = (Operator*) calloc(1, sizeof(Operator));
    op->type = OP_FILTER;
    op->next                    = NULL;
    op->child                   = NULL;
    op->getTuple                = NULL;

    op->info.filter.boolExprListSize = 0;

    memcpy(&op->resultDescription, &child->resultDescription, sizeof(child->resultDescription));


    mapBoolExpr(
        node->child,
        &child->resultDescription,
        op->info.filter.boolExprList,
        &op->info.filter.boolExprListSize
    );

    boolExprAddTypes(
        node->child,
        op->info.filter.exprTypes
    );

    boolExprAddConstants(
        node->child,
        op->info.filter.charConstants
    );

    return op;
}


Operator* makeProjectOp(Node* node) {

    Operator* op = (Operator*) calloc(1, sizeof(Operator));
    op->type = OP_PROJECT;
    op->next    = NULL;
    op->child   = NULL;

    int i = 0;

    while (node->next != NULL)  {
        
        op->info.project.colCount++;
        strcpy(op->info.project.columnsToProject[i], node->content);
        op->info.project.colRefs[i] = node->colRef;
        op->info.project.tblRefs[i] = node->tableRef;
        op->resultDescription.columns[i].type = node->dtype;
        op->resultDescription.columns[i].identifier = node->identifier;
        i++;
        node = node->next;

        
    } ;


    // TODO. The last column is not handled by the loop, so 
    // the loop body is repeated
    op->info.project.colCount++;
    strcpy(op->info.project.columnsToProject[i], node->content);
    op->info.project.colRefs[i] = node->colRef;
    op->info.project.tblRefs[i] = node->tableRef;
    op->resultDescription.columns[i].type = node->dtype;
    op->resultDescription.columns[i].identifier = node->identifier;
    i++;

    op->resultDescription.columnCount = op->info.project.colCount;


    return op;
}




Operator* planQuery(Node* astRoot, TableMetadata* tables, size_t tableCount) {

    /* 
        Building the QueryPlan which is a tree of operators.
        The tree is built from the ground up. 

        - Each reference to a physical table is a leaf node
        - The root if the tree is always an project operation
    
    */

    Node* SELECT = astRoot->next;
    Operator* op_proj = makeProjectOp(SELECT->child);

    if (tableCount > 1) {
        printf("More than one table. Not implemented.\n");
        exit(1);
    }
    
    Operator* op_scan = makeScanOp(tables[0]);
    Node* WHERE = astRoot->next->next->next;

    if (WHERE != NULL) {

        Operator* op_filt = makeFilterOp(WHERE, op_scan);
        op_scan->next   = op_filt;
        op_filt->next   = op_proj;

        op_proj->child  = op_filt;
        op_filt->child  = op_scan;

    } else {
        op_scan->next  = op_proj;
        op_proj->child = op_scan;
    }


    return op_proj;   
}