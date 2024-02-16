#include "../include/operators/aggregate.h"




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
    
    Tuple* tpl = op->child->getTuple(op->child);
    long result = 0;
    while (tpl != NULL) {
        tpl = op->child->getTuple(op->child);
        result++;
    };

    // Build new tuple

    tplbuffer->tupleCount++;
    size_t idx = tplbuffer->tupleCount;    

    sprintf(buffercache, "%ld", result);
    size_t numSizeAsChar = strlen(buffercache);
    
    tplbuffer->tuples[idx].pCols[0] = buffercache;
    tplbuffer->tuples[idx].size = numSizeAsChar;
    tplbuffer->tuples[idx].columnCount = 1;

    buffercache += numSizeAsChar;
    op->info.aggregate.aggregationDone = true;
    
    return &tplbuffer->tuples[idx];

}
