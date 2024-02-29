#include "../include/operators/aggregate.h"

long doCount(Operator* opToIterate) {
    Tuple* tpl = getTuple(opToIterate->getTuple(opToIterate));
    int result = 0;
    while (tpl != NULL) {
        tpl = getTuple(opToIterate->getTuple(opToIterate));
        result++;
    };
    
    return result;
}

long doAverage(Operator* opToIterate, int colIdx) {

    if (opToIterate->resultDescription.columns[colIdx].type != DTYPE_LONG) {
        printf("AVG not implement for other datatypes besides long\n");
        exit(1);
    }
    Tuple* tpl = NULL;
    long sum = 0;
    long count = 0;

    for (;;) {
        tpl = getTuple(opToIterate->getTuple(opToIterate));
        if (tpl == NULL) {
            break;
        }
        sum += *(long*) getCol(tpl,colIdx);
        count++;
    };
    long result = 0.0; 
    if (count > 0) {
        result = sum / (double) count;
    }
    return result;
}

long doSum(Operator* opToIterate, int colIdx) {

    if (opToIterate->resultDescription.columns[colIdx].type != DTYPE_LONG) {
        printf("Sum not implement for other datatypes besides long\n");
        exit(1);
    }

    Tuple* tpl = NULL;
    long result = 0;

    for (;;) {
        tpl = getTuple(opToIterate->getTuple(opToIterate));
        if (tpl == NULL) {
            break;
        }
        result += *(long*) getCol(tpl,colIdx);
    };

    return result;
}


int aggregateGetTuple(Operator* op) {
    
    checkPtrNotNull(op->child, "OP_AGGREGATE has no child.");
    checkPtrNotNull(op->child->getTuple, "Child of OP_AGGREGATE has no getTuple-method.");

    if (op->info.aggregate.aggregationDone) {
        return -1;
    }

    // Build new tuple to store result

    int result = 0;

    switch(op->info.aggregate.aggtype) {
        case COUNT:
            result = doCount(op->child);
            break;
        case SUM:
            result = doSum(op->child, op->info.aggregate.colToAggregate);
            break;
        case AVG:
            result = doAverage(op->child, op->info.aggregate.colToAggregate);
            break;
        default:
            printf("Aggregation type (%d) not implemented\n", op->info.aggregate.aggtype);
            exit(1);
    }
 
    Tuple* tpl = addTuple();
    memcpy(tpl->data, &result, sizeof(result));

    tpl->columnCount = 1;
    tpl->pCols[0] = 0;
    
    op->info.aggregate.aggregationDone = true;
    
    return tpl->idx;

}
