#include "../include/operators/filter.h"


bool evaluateTupleAgainstFilterOp(int poolOffset, Operator* op) {

    if (poolOffset == -1) {
        return false;
    }

    
    int idx1    = op->info.filter.boolExprList[0];
    int boolOp  = op->info.filter.boolExprList[1];
    int idx2    = op->info.filter.boolExprList[2];

    enum nodeType type1 = op->info.filter.exprTypes[0];
    enum nodeType type2 = op->info.filter.exprTypes[2];
    
    int idx1Offset = op->resultDescription.pCols[idx1];
    int idx2Offset = op->resultDescription.pCols[idx2];

    if (
        idx1 > (int) op->resultDescription.columnCount
        ||
        idx2 > (int) op->resultDescription.columnCount
    ) {
        printf("FILTER_OP: Filter column references (%d | %d  > %ld) out of bounds\n", idx1, idx2, op->resultDescription.columnCount);
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
            3. 1 is column, 1 is constant
    */

    // 1. Both are columns. They must be of the same datatype
    if (type1 == IDENT_COL && type2 == IDENT_COL)   {

        if (dtype1 != dtype2) {
            printf("FILTER_OP: Can't compare different datatypes\n");
            exit(1);
        }

        switch (dtype1)   {
            case DTYPE_STR:
                cmpRes = strcmp(
                    (char*) getCol(poolOffset,idx1Offset),
                    (char*) getCol(poolOffset,idx2Offset)
                );
                break;
            case DTYPE_INT:
                int number1 = *(int*) getCol(poolOffset,idx1Offset);
                int number2 = *(int*) getCol(poolOffset,idx2Offset);
                cmpRes = number1 - number2;
                break;
            case DTYPE_LONG:
            
                long lnumber1 = *(long*) getCol(poolOffset,idx1Offset);
                long lnumber2 = *(long*) getCol(poolOffset,idx2Offset);
                cmpRes = lnumber1 - lnumber2;
                break;
            default:
                printf("FILTER_OP: Don't know how to compare datatype %d\n", dtype1);
                exit(1);
        }
    }
    // 2. Both are constants
    else if (type1 != IDENT_COL && type2 != IDENT_COL) {
        if (nodeDtype1 != nodeDtype2) {
            printf("FILTER_OP: Can't compare different datatypes\n");
            exit(1);
        }
        switch (nodeDtype1)   {
            case DTYPE_STR:
                cmpRes = strcmp(op->info.filter.charConstants[0], op->info.filter.charConstants[2]);
                break;
            case DTYPE_INT:
            case DTYPE_LONG:
                cmpRes = op->info.filter.numConstants[0] - op->info.filter.numConstants[2];
                break;
            default:
                printf("FILTER_OP: Don't know how to compare datatype %d\n", nodeDtype1);
                exit(1);
        }
    }
    // 3. One is a constant, one is a column
    else {
        // Guess 1st is a column and 2nd is constant
        // and fix if it's not
        
        Datatype colDatatype    = dtype1;
        Datatype constDatatype  = nodeDtype2;
        size_t colOffset   = idx1Offset;
        size_t constIdx = 2;
        
        if (type2 == IDENT_COL) {
            // Guess was wrong, fix it
            constDatatype   = nodeDtype1;
            colDatatype     = dtype2;
            constIdx        = 0;
            colOffset          = idx2Offset;
        }
        if (constDatatype != colDatatype) {
            printf("FILTER_OP: 3. Don't know how to compare datatypes %d vs %d\n", constDatatype, colDatatype);
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

bool evaluateTupleAgainstFilterOps(int poolOffset, Operator* op) {

    bool rtrnValue = true,
         result = true;

    enum nodeType boolOp = AND;
    Operator* p_op = op;

    while (p_op != NULL) {
        
        result = evaluateTupleAgainstFilterOp(poolOffset, p_op);

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

int filterGetTuple(Operator* op) {

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


    int poolOffset = 0;

    while (true) {
        /* Get new tuples until found something that passes the filter */

        poolOffset = op->child->getTuple(op->child);

        if (poolOffset == -1) {
            return -1;
        }

        if (evaluateTupleAgainstFilterOps(poolOffset, op)) break;


    }
    return poolOffset;
}
