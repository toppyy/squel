#include "../include/operators/filter.h"

bool evaluateTupleAgainsFilterOp(Tuple* tpl, Operator* op) {

    if (tpl == NULL) {
        return NULL;
    }

    
    int idx1    = op->info.filter.boolExprList[0];
    int boolOp  = op->info.filter.boolExprList[1];
    int idx2    = op->info.filter.boolExprList[2];

    enum nodeType type1 = op->info.filter.exprTypes[0];
    enum nodeType type2 = op->info.filter.exprTypes[2];


    if (
        idx1 > (int) tpl->columnCount
        ||
        idx2 > (int) tpl->columnCount 
    ) {
        printf("Filter column references out of bounds\n");
        exit(1);
    }


    bool matches = false;

    int cmpRes = 0;
    
    if (type1 == IDENT_COL && type2 == IDENT_COL)       cmpRes = strcmp(tpl->pCols[idx1],tpl->pCols[idx2]);
    else if (type1 == IDENT_COL && type2 == STRING)     cmpRes = strcmp(tpl->pCols[idx1],op->info.filter.charConstants[2]);
    else if (type1 == STRING && type2 == IDENT_COL)     cmpRes = strcmp(op->info.filter.charConstants[0], tpl->pCols[idx2]);
    else if (type1 == IDENT_COL && type2 == NUMBER) {
        int number = atoi(tpl->pCols[idx1]);
        cmpRes = number - op->info.filter.intConstants[2];
    }
    else if (type1 == NUMBER && type2 == IDENT_COL) {
        int number = atoi(tpl->pCols[idx2]);
        cmpRes = op->info.filter.intConstants[0] - number;
    } 
    

    switch(boolOp) {
        case -1:
            matches = cmpRes == 0;
            break;
        case -2:
            matches = cmpRes != 0;
            break;
        case -3:
            matches = cmpRes < 0;
            break;
        case -4:
            matches = cmpRes > 0;
            break;
        default:    
            printf("Operator %d not implemented\n", boolOp);
            exit(1);
    }

    return matches;
}

Tuple* filterGetTuple(Operator* op) {

    if (op == NULL) {
        printf("Passed a NULL-pointer to filterGetTuple\n");
        exit(1);
    }

    if (op->type != OP_FILTER) {
        printf("Called filterGetTuple on an operator that is not OP_FILTER\n");
    }

    if (op->child == NULL) {
        printf("OP_FILTER has no child\n");
        exit(1);
    }

    if ( op->child->getTuple == NULL) {
        printf("Child of OP_FILTER has no getTuple-method\n");
        exit(1);
    }

    Tuple* tpl = NULL;

    while (true) {
        /* Get new tuples until found something that passes the filter */

        tpl = op->child->getTuple(op->child);

        if (tpl == NULL) {
            return false;
        }

        if (evaluateTupleAgainsFilterOp(tpl, op)) break;


    }

    return tpl;
}
