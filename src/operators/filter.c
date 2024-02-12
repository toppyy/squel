#include "../include/operators/filter.h"


Datatype mapNodeTypeToDataType(enum nodeType type) {
    switch (type) {
        case NUMBER:
            return DTYPE_INT;
            break;
        case STRING:
            return DTYPE_STR;
            break;
        default:
            return DTYPE_UNDEFINED;
            break;
    }
}

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


    int cmpRes = 0;

    /* Node datatype is inferred from nodetype. Applies to constants only. */
    Datatype nodeDtype1 = mapNodeTypeToDataType(type1);
    Datatype nodeDtype2 = mapNodeTypeToDataType(type2);

    /* Column datatype is inferred from data. */
    Datatype dtype1 = op->resultDescription.columns[idx1].type;
    Datatype dtype2 = op->resultDescription.columns[idx2].type;

    /*
        Three cases:
            1. Both are columns
            2. Neither is a column
            3. 1 is column, 2 is constant
    */

    // 1. Both are columns. They must be of the same datatype
    if (type1 == IDENT_COL && type2 == IDENT_COL)   {

        if (dtype1 != dtype2) {
            printf("Can't compare different datatypes\n");
            exit(1);
        }

        switch (dtype1)   {
            case DTYPE_STR:
                cmpRes = strcmp(tpl->pCols[idx1],tpl->pCols[idx2]);
                break;
            case DTYPE_INT:
                int number1 = atoi(tpl->pCols[idx1]);
                int number2 = atoi(tpl->pCols[idx2]);
                cmpRes = number1 - number2;
                break;
            default:
                printf("Don't know how to compare datatypes %d vs .%d\n", dtype1, dtype2);
                exit(1);
        }
    }
    // 2. Both are constants
    else if (type1 != IDENT_COL && type2 != IDENT_COL) {
        if (nodeDtype1 != nodeDtype2) {
            printf("Can't compare different datatypes\n");
            exit(1);
        }
        switch (dtype1)   {
            case DTYPE_STR:
                cmpRes = strcmp(op->info.filter.charConstants[0], op->info.filter.charConstants[2]);
                break;
            case DTYPE_INT:
                cmpRes = op->info.filter.intConstants[0] - op->info.filter.intConstants[2];
                break;
            default:
                printf("Don't know how to compare datatypes %d vs %d\n", nodeDtype1, nodeDtype2);
                exit(1);
        }
    }
    // 3. One is a constant, one is a column
    else {
        // Guess 1st is a column and 2nd is constant
        // and fix if it's not
        
        Datatype colDatatype    = dtype1;
        Datatype constDatatype  = nodeDtype2;
        size_t colIdx   = idx1;
        size_t constIdx = 2;
        
        if (type2 == IDENT_COL) {
            // Guess was wrong, fix it
            // printf("Got it wrong\n");
            constDatatype   = nodeDtype1;
            colDatatype     = dtype2;
            constIdx        = 0;
            colIdx          = idx2;
        }
        if (constDatatype != colDatatype) {
            printf("Don't know how to compare datatypes %d vs %d\n", constDatatype, colDatatype);
            exit(1);
        }
        // Now we have to only deal with correct combinations of all the eight possible
        // That is:
        //      IDENT_COL + STRING vs. DTYPE_STR
        //      IDENT_COL + NUMBER vs. DTYPE_INT
        //      DTYPE_STR vs. IDENT_COL + STRING
        //      DTYPE_INT vs. IDENT_COL + NUMBER
        switch (constDatatype) {
            case DTYPE_STR:
                cmpRes = strcmp(op->info.filter.charConstants[constIdx], tpl->pCols[colIdx]);
                break;
            case DTYPE_INT:
                int colNumber = atoi(tpl->pCols[colIdx]);
                int constNumber = op->info.filter.intConstants[constIdx];
                // printf("Comparing %d to %d\n", colNumber, constNumber);
                // Order matters here
                if (constIdx == 0) {
                    cmpRes = constNumber - colNumber;
                } else {
                    cmpRes = colNumber - constNumber;
                }
                break;        
            default:
                printf("Don't know how to handle datatype %d\n", constDatatype);
                exit(1);
        }
    }
    
    bool matches = false;
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
