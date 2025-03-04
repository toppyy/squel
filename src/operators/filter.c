#include "../include/operators/filter.h"


bool evaluateTupleAgainstFilterOp(int poolOffset1, int poolOffset2, Operator* op) {

    if (poolOffset1 == -1) {
        return false;
    }

    if (poolOffset2 == -1) {
        return false;
    }

    
    int idx1        = op->info.filter.boolExprList[0];
    int boolOp      = op->info.filter.boolExprList[1];
    int idx2        = op->info.filter.boolExprList[2];    
    int idx1Offset  = op->resultDescription.pCols[idx1];
    int idx2Offset  = op->resultDescription.pCols[idx2];
    ComparisonType compType = op->info.filter.compType;


    int cmpRes = 0;

    Datatype dtype1 = op->info.filter.exprDatatypes[0];
    Datatype dtype2 = op->info.filter.exprDatatypes[1];

    if (dtype1 != dtype2) {
        printf("FILTER_OP: Can't compare different datatypes\n");
        exit(1);
    }


    /*
        Three cases:
            1. Both are columns (CMP_COL_COL)
            2. Neither is a column (CMP_CONST_CONST)
            3. 1 is column, 1 is constant (CMP_CONST_COL | CMP_COL_CONST)
    */

    // 1. Both are columns
    if (compType == CMP_COL_COL)   {

        switch (dtype1)   {
            case DTYPE_STR:
                cmpRes = strcmp(
                    (char*) getCol(poolOffset1,idx1Offset),
                    (char*) getCol(poolOffset2,idx2Offset)
                );
                break;
            case DTYPE_INT:
                int number1 = *(int*) getCol(poolOffset1,idx1Offset);
                int number2 = *(int*) getCol(poolOffset2,idx2Offset);
                cmpRes = number1 - number2;
                break;
            case DTYPE_LONG:
                long lnumber1 = *(long*) getCol(poolOffset1,idx1Offset);
                long lnumber2 = *(long*) getCol(poolOffset2,idx2Offset);
                cmpRes = lnumber1 - lnumber2;
                break;
            default:
                printf("FILTER_OP: Don't know how to compare datatype %d\n", dtype1);
                exit(1);
        }
    }
    // 2. Both are constants
    else if (compType == CMP_CONST_CONST) {

        switch (dtype1)   {
            case DTYPE_STR:
                cmpRes = strcmp(op->info.filter.charConstants[0], op->info.filter.charConstants[2]);
                break;
            case DTYPE_INT:
            case DTYPE_LONG:
                cmpRes = op->info.filter.numConstants[0] - op->info.filter.numConstants[2];
                break;
            default:
                printf("FILTER_OP: Don't know how to compare datatype %d\n", dtype1);
                exit(1);
        }
    }
    // 3. One is a constant, one is a column
    else {
        // Guess 1st is a column and 2nd is constant
        // and fix if it's not
        
        Datatype constDatatype  = dtype2;
        size_t colOffset   = idx1Offset;
        size_t constIdx = 2;
        int poolOffset = poolOffset1;
        
        if (compType == CMP_CONST_COL) {
            // Guess was wrong, fix it
            constDatatype   = dtype1;
            constIdx        = 0;
            colOffset       = idx2Offset;
            poolOffset      = poolOffset2;
        }
        // Now we have to only deal with 4 combinations of all the eight possible
        // 'cause datatypes must match
        // That is:
        //      IDENT_COL + STRING vs. DTYPE_STR
        //      IDENT_COL + NUMBER vs. DTYPE_INT
        //      DTYPE_STR vs. IDENT_COL + STRING
        //      DTYPE_INT vs. IDENT_COL + NUMBER
        switch (constDatatype) {
            case DTYPE_STR:
                cmpRes = strcmp(op->info.filter.charConstants[constIdx], getCol(poolOffset,colOffset));
                break;
            case DTYPE_LONG:
                long colNumber = *(long*) getCol(poolOffset,colOffset);
                long constNumber = (long) op->info.filter.numConstants[constIdx];
                // Order matters here
                if (constIdx == 0) {
                    cmpRes = constNumber - colNumber;
                } else {
                    cmpRes = colNumber - constNumber;
                }
                break;        
            default:
                printf("FILTER_OP: Don't know how to handle datatype %d\n", constDatatype);
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
            printf("FILTER_OP: Operator %d not implemented\n", boolOp);
            exit(1);
    }

    return matches;
}

bool evaluateTuplesAgainstFilterOps(int poolOffset1, int poolOffset2, Operator* op) {

    bool rtrnValue = true,
         result = true;

    enum nodeType boolOp = AND;
    Operator* p_op = op;

    while (p_op != NULL) {
        
        result = evaluateTupleAgainstFilterOp(poolOffset1, poolOffset2, p_op);

        switch (boolOp) {
            case AND:
                rtrnValue = result && rtrnValue;            
                break;
            case OR:
                rtrnValue = result || rtrnValue;
                break;
            default:
                printf("FILTER_OP: Don't know how evaluate bool. op %d\n", boolOp);
                exit(1);
        }
        boolOp = p_op->info.filter.operatorNext;
        p_op = p_op->info.filter.next;
        
    }

    return rtrnValue;
}

Tuple* filterGetTuple(Operator* op) {

    if (op == NULL) {
        printf("FILTER_OP: Passed a NULL-pointer to filterGetTuple\n");
        exit(1);
    }

    if (op->type != OP_FILTER) {
        printf("FILTER_OP: Called filterGetTuple on an operator that is not OP_FILTER\n");
    }

    if (op->child == NULL) {
        printf("FILTER_OP: OP_FILTER has no child\n");
        exit(1);
    }

    if ( op->child->getTuple == NULL) {
        printf("FILTER_OP: Child of OP_FILTER has no getTuple-method\n");
        exit(1);
    }


    Tuple* tpl = 0;

    while (true) {
        /* Get new tuples until found something that passes the filter */

        tpl = op->child->getTuple(op->child);

        if (tpl == NULL) {
            break;
        }

        if (evaluateTuplesAgainstFilterOps(0, 0, op)) break;


    }

    return tpl;
}
