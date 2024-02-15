#include "../../include/planner/planner.h"

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
