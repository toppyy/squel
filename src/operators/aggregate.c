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
    
    checkPtrNotNull(op->child, "OP_AGGREGATE has no child.");
    checkPtrNotNull(op->child->getTuple, "Child of OP_AGGREGATE has no getTuple-method.");

    if (op->info.aggregate.aggregationDone) {
        return NULL;
    }

    // Build new tuple to store result
 
    Tuple* tpl = addTuple();    
    tpl->columnCount = 1;


    char* resultLocation = NULL;

    switch(op->info.aggregate.aggtype) {
        case COUNT:
            resultLocation = doCount(op->child, tpl->data);
            break;
        case SUM:
            resultLocation = doSum(op->child, op->info.aggregate.colToAggregate, tpl->data);
            break;
        case AVG:
            resultLocation = doAverage(op->child, op->info.aggregate.colToAggregate, tpl->data);
            break;
        default:
            printf("Aggregation type (%d) not implemented\n", op->info.aggregate.aggtype);
            exit(1);
    }

    tpl->pCols[0] = resultLocation;
    tpl->size = strlen(resultLocation);
    
    
   


    op->info.aggregate.aggregationDone = true;
    
    return tpl;

}
