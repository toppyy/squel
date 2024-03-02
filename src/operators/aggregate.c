#include "../include/operators/aggregate.h"

long doCount(Operator* opToIterate) {
    int offset = opToIterate->getTuple(opToIterate);
    int result = 0;
    while (offset >= 0) {
        offset = opToIterate->getTuple(opToIterate);
        result++;
    };
    
    return result;
}

long doAverage(Operator* opToIterate, size_t colOffset) {


    int offset = 0;
    long sum = 0;
    long count = 0;

    for (;;) {
        offset = opToIterate->getTuple(opToIterate);
        if (offset == -1) {
            break;
        }
        sum += *(long*) getCol(offset,colOffset);
        count++;
    };
    long result = 0.0; 
    if (count > 0) {
        result = sum / (double) count;
    }
    return result;
}

long doSum(Operator* opToIterate, size_t colOffset) {


    int offset = 0;
    long result = 0;

    for (;;) {
        offset = opToIterate->getTuple(opToIterate);
        if (offset == -1) {
            break;
        }
        result += *(long*) getCol(offset,colOffset);

    };

    return result;
}


int aggregateGetTuple(Operator* op) {
    
    checkPtrNotNull(op->child, "OP_AGGREGATE has no child.");
    checkPtrNotNull(op->child->getTuple, "Child of OP_AGGREGATE has no getTuple-method.");

    if (op->info.aggregate.aggregationDone) {
        return -1;
    }

    // TODO:
    //     if (opToIterate->resultDescription.columns[colIdx].type != DTYPE_LONG) {
    //     printf("Sum not implement for other datatypes besides long\n");
    //     exit(1);
    // }


    // Build new tuple to store result

    int result = 0;

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
        default:
            printf("Aggregation type (%d) not implemented\n", op->info.aggregate.aggtype);
            exit(1);
    }

    op->resultDescription.columnCount = 1;
    op->resultDescription.pCols[0] = 0;
    op->info.aggregate.aggregationDone = true;
    

    return addToBufferPool(&result, sizeof(result));
}
