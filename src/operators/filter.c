#include "../include/operators/filter.h"

bool evaluateTupleAgainstFilterOp(Tuple* tpl1, Tuple* tpl2, Operator* op) {

    int idx1        = op->info.filter.boolExprList[0];
    int boolOp      = op->info.filter.boolExprList[1];
    int idx2        = op->info.filter.boolExprList[2];    

    ComparisonType compType = op->info.filter.compType;
    Datatype dtype1 = op->info.filter.exprDatatypes[0];
    Datatype dtype2 = op->info.filter.exprDatatypes[1];    

    if (dtype1 != dtype2) {
        printf("FILTER_OP: Can't compare different datatypes\n");
        exit(1);
    }

    
    /*
    Four cases:
        1. Both are columns (CMP_COL_COL)
        2. Neither is a column (CMP_CONST_CONST)
        3. column vs. constant (CMP_COL_CONST)
        4. column vs. constant (CMP_CONST_COL)
    */

    int cmpRes = 0;
    long colNumber, constNumber;

    // 1. Both are columns
    if (compType == CMP_COL_COL)   {

        switch (dtype1)   {
            case DTYPE_STR:
                cmpRes = strcmp(
                    (char*) getTupleColByIndex(tpl1,idx1),
                    (char*) getTupleColByIndex(tpl2,idx2)
                );
                break;

            case DTYPE_INT:

                int number1 = *(int*) getTupleColByIndex(tpl1,idx1);
                int number2 = *(int*) getTupleColByIndex(tpl2,idx2);
                cmpRes = number1 - number2;
                break;

            case DTYPE_LONG:

                long lnumber1 = getTupleLongColByIndex(tpl1,idx1);
                long lnumber2 = getTupleLongColByIndex(tpl2,idx2);
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
    // 3. Left is a column, right is a const
    else if (compType == CMP_COL_CONST) {
        
        size_t constIdx = 2;
        size_t i = idx1;
        Tuple* tpl = tpl1;
        
        switch (dtype2) {
            case DTYPE_STR:
                cmpRes = strcmp(op->info.filter.charConstants[constIdx], getTupleColByIndex(tpl,i));
                break;

            case DTYPE_LONG:            
                colNumber      = getTupleLongColByIndex(tpl,i);
                constNumber    = op->info.filter.numConstants[constIdx];
                cmpRes = colNumber - constNumber;
                break;

            default:
                printf("FILTER_OP: Don't know how to handle datatype %d\n", dtype2);
                exit(1);
        }
    }
    
    else if (compType == CMP_CONST_COL) {

        size_t constIdx = 0;
        size_t i = idx2;
        Tuple* tpl = tpl2;

        
        switch (dtype1) {
            case DTYPE_STR:
                cmpRes = strcmp(op->info.filter.charConstants[constIdx], getTupleColByIndex(tpl,i));
                break;

            case DTYPE_LONG:
                colNumber      = getTupleLongColByIndex(tpl,i);
                constNumber    = op->info.filter.numConstants[constIdx];
                cmpRes = constNumber - colNumber;
                break;

            default:
                printf("FILTER_OP: Don't know how to handle datatype %d\n", dtype1);
                exit(1);
        }

    }

    switch(boolOp) {
        case -1:
            return cmpRes == 0;
        case -2:
            return cmpRes != 0;
        case -3:
            return cmpRes < 0;
        case -4:
            return cmpRes > 0;
        default:    
            printf("FILTER_OP: Operator %d not implemented\n", boolOp);
            exit(1);
    }

}




bool evaluateTuplesAgainstFilterOps(Tuple* tpl1, Tuple* tpl2, Operator* op) {

    bool rtrnValue = true,
         result = true;

    enum nodeType boolOp = AND;
    Operator* p_op = op;

    while (p_op != NULL) {
        
        result = evaluateTupleAgainstFilterOp(tpl1, tpl2, p_op);

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

void filterGetTuple(Operator* op, Tuple* tpl) {

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
    }if (op == NULL) {
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

    while (true) {
        /* Get new tuples until found something that passes the filter */

        op->child->getTuple(op->child, tpl);

        if (isTupleEmpty(tpl)) {
            break;
        }

        if (evaluateTuplesAgainstFilterOps(tpl, tpl, op)) break;
    }
}
