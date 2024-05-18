#include "../../include/planner/planner.h"

Datatype mapNodeTypeToDataType(enum nodeType type) {
    switch (type) {
        case NUMBER:
            return DTYPE_LONG;
            break;
        case STRING:
            return DTYPE_STR;
            break;
        default:
            return DTYPE_UNDEFINED;
            break;
    }
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


Node* mapBoolExpr(
    Node* node,
    ResultSet* childResultDesc,
    int* boolExprList,
    size_t* boolExprListSize
) {
    checkPtrNotNull(node, "Passed NULL-pointer to mapBoolExpr.");
    checkPtrNotNull(node->next, "Making bool expr failed. Node has no next.");
    checkPtrNotNull(node->next->next, "Making bool expr failed. Node's next has no next.");

    Node* value1    = node;
    Node* op        = node->next;
    Node* value2    = node->next->next;

    if (op->type != BOOLOP) {
        printf("Should've been a bool operator, but was %d (content %s)\n", op->type, op->content);
        exit(1);
    }


    int value1Loc = value1->type == IDENT_COL ? findColIdxInResDesc(childResultDesc, value1->content, value1->tblref) : -1;
    int value2Loc = value2->type == IDENT_COL ? findColIdxInResDesc(childResultDesc, value2->content, value2->tblref) : -1;


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

void boolExprAddDatatypes(Operator *op, Node* node) {

    int idx1    = op->info.filter.boolExprList[0];
    int idx2    = op->info.filter.boolExprList[2];

    if (idx1 >= 0) {
        op->info.filter.exprDatatypes[0] = op->resultDescription.columns[idx1].type;
    } else {
        op->info.filter.exprDatatypes[0] = mapNodeTypeToDataType(node->type);
    }

    if (idx2 >= 0) {
        op->info.filter.exprDatatypes[1] = op->resultDescription.columns[idx2].type;
    } else {
        op->info.filter.exprDatatypes[1] = mapNodeTypeToDataType(node->next->next->type);
    }

}

void boolExprAddConstants(Node* node, char (*charConstants)[CHARMAXSIZE], long* numConstants) {

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
            numConstants[i] = atol(p_node->content);
        }
            
        p_node = p_node->next;
        i++;
    }


}

ComparisonType deduceComparisonType(enum nodeType type1, enum nodeType type2) {
    if (type1 == IDENT_COL && type2 == IDENT_COL)   {
        return CMP_COL_COL;
    }
    if (type1 == IDENT_COL && type2 != IDENT_COL)   {
        return CMP_COL_CONST;
    }
    if (type1 != IDENT_COL && type2 == IDENT_COL)   {
        return CMP_CONST_COL;
    }
    return CMP_CONST_CONST;
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
        op->info.filter.numConstants
    );


    boolExprAddDatatypes(
        op,
        node
    );

    
    op->info.filter.compType = deduceComparisonType(op->info.filter.exprTypes[0],op->info.filter.exprTypes[2]);

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
