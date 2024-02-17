#include "../include/operators/aggregate.h"

char* doCount(Operator* opToIterate, char* resultBuffer) {
    Tuple* tpl = opToIterate->getTuple(opToIterate);
    long result = 0;
    while (tpl != NULL) {
        tpl = opToIterate->getTuple(opToIterate);
        result++;
    };
    sprintf(resultBuffer, "%ld", result);
    return resultBuffer;
}

char* doAverage(Operator* opToIterate, int colIdx, char* resultBuffer) {
    Tuple* tpl = NULL;
    long sum = 0;
    long count = 0;

    for (;;) {
        tpl = opToIterate->getTuple(opToIterate);
        if (tpl == NULL) {
            break;
        }
        if (opToIterate->resultDescription.columns[colIdx].type == DTYPE_INT) {
            sum += atol(tpl->pCols[colIdx]);
            count++;
        } else {
            printf("Sum not implement for non-integers\n");
            exit(1);
        }
    };
    double result = 0.0; 
    if (count > 0) {
        result = sum / (double) count;
    }
    sprintf(resultBuffer, "%.2f", result);
    return resultBuffer;
}




char* doSum(Operator* opToIterate, int colIdx, char* resultBuffer) {

    Tuple* tpl = NULL;
    long result = 0;

    for (;;) {
        tpl = opToIterate->getTuple(opToIterate);
        if (tpl == NULL) {
            break;
        }
        if (opToIterate->resultDescription.columns[colIdx].type == DTYPE_INT) {
            result += atol(tpl->pCols[colIdx]);
        } else {
            printf("Sum not implement for non-integers\n");
            exit(1);
        }
    };
    sprintf(resultBuffer, "%ld", result);
    return resultBuffer;
}


Tuple* aggregateGetTuple(Operator* op) {
    if (op->child == NULL) {
        printf("OP_AGGREGATE has no child\n");
        exit(1);
    }

    if ( op->child->getTuple == NULL) {
        printf("Child of OP_AGGREGATE has no getTuple-method\n");
        exit(1);
    }

    if (op->info.aggregate.aggregationDone) {
        return NULL;
    }

    char* resultLocation = NULL;

    switch(op->info.aggregate.aggtype) {
        case COUNT:
            resultLocation = doCount(op->child, buffercache);
            break;
        case SUM:
            resultLocation = doSum(op->child, op->info.aggregate.colToAggregate, buffercache);
            break;
        case AVG:
            resultLocation = doAverage(op->child, op->info.aggregate.colToAggregate, buffercache);
            break;
        default:
            printf("Aggregation type (%d) not implemented\n", op->info.aggregate.aggtype);
            exit(1);
    }
    
    
    // Build new tuple

    tplbuffer->tupleCount++;
    size_t idx = tplbuffer->tupleCount;
    size_t numSizeAsChar = strlen(resultLocation);
    
    tplbuffer->tuples[idx].pCols[0] = resultLocation;
    tplbuffer->tuples[idx].size = numSizeAsChar + 1;
    tplbuffer->tuples[idx].columnCount = 1;


    op->info.aggregate.aggregationDone = true;
    
    return &tplbuffer->tuples[idx];

}
