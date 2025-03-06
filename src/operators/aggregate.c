#include "../include/operators/aggregate.h"

long doCount(Operator* opToIterate) {
    int result = 0;
    while (true) {
        Tuple* tpl = opToIterate->getTuple(opToIterate);
        if (tpl == NULL) break;
        result++;
        freeTuple(tpl);
    };
    
    return result;
}

long doAverage(Operator* opToIterate, size_t colOffset) {


    long sum = 0;
    long count = 0;

    for (;;) {
        Tuple* tpl = opToIterate->getTuple(opToIterate);
        if (tpl == NULL) {
            break;
        }
        sum += *(long*) (tpl->data + colOffset);
        count++;
        freeTuple(tpl);
    };
    long result = 0.0; 
    if (count > 0) {
        result = sum / (double) count;
    }
    return result;
}

long doSum(Operator* opToIterate, size_t colOffset) {


    long long result = 0;

    for (;;) {
        Tuple* tpl = opToIterate->getTuple(opToIterate);
        if (tpl == NULL) {
            break;
        }
        result += *(long*) (tpl->data + colOffset);
        freeTuple(tpl);
    };

    return result;
}

long doMax(Operator* opToIterate, size_t colOffset) {


    long result = 0, tmp = 0;

    for (;;) {
        Tuple* tpl = opToIterate->getTuple(opToIterate);
        if (tpl == NULL) {
            break;
        }
        tmp = *(long*) (tpl->data + colOffset);
        result = tmp > result ? tmp : result;
        freeTuple(tpl);

    };

    return result;
}

long doMin(Operator* opToIterate, size_t colOffset) {


    long result = __LONG_MAX__, tmp = 0;

    for (;;) {
        Tuple* tpl = opToIterate->getTuple(opToIterate);
        if (tpl == NULL) {
            break;
        }
        tmp = *(long*) (tpl->data + colOffset);
        result = tmp < result ? tmp : result;
        freeTuple(tpl);

    };

    return result;
}



Tuple* aggregateGetTuple(Operator* op) {
    
    checkPtrNotNull(op->child, "OP_AGGREGATE has no child.");
    checkPtrNotNull(op->child->getTuple, "Child of OP_AGGREGATE has no getTuple-method.");

    if (op->info.aggregate.aggregationDone) {
        return NULL;
    }

    // TODO:
    //     if (opToIterate->resultDescription.columns[colIdx].type != DTYPE_LONG) {
    //     printf("Sum not implement for other datatypes besides long\n");
    //     exit(1);
    // }


    // Build new tuple to store result

    long result = 0;

    switch(op->info.aggregate.aggtype) {
        case COUNT:
            result = doCount(op->child);
            break;
        case SUM:
            result = doSum(op->child, op->child->resultDescription.pCols[op->info.aggregate.colToAggregate]);
            break;
        case AVG:
            result = doAverage(op->child, op->child->resultDescription.pCols[op->info.aggregate.colToAggregate]);
            break;
        case MAX:
            result = doMax(op->child, op->child->resultDescription.pCols[op->info.aggregate.colToAggregate]);
            break;
        case MIN:
            result = doMin(op->child, op->child->resultDescription.pCols[op->info.aggregate.colToAggregate]);
            break;
        default:
            printf("Aggregation type (%d) not implemented\n", op->info.aggregate.aggtype);
            exit(1);
    }

    op->resultDescription.columnCount = 1;
    op->resultDescription.pCols[0] = 0;
    op->info.aggregate.aggregationDone = true;
    
    Tuple* tpl = initTuple();
    long* res_ptr = malloc(sizeof(result));
    *res_ptr = result;
    tpl->data = res_ptr;

    return tpl;
}
