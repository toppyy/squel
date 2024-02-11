#include "../include/operators/join.h"


Tuple* concat_tuples(Tuple* left, Tuple* right) {

    if (
        left == NULL ||
        right == NULL
    ) {
        printf("Passed a NULL pointer to concat_tuples\n");
        exit(1);
    }

    tplbuffer->tupleCount++;
    size_t idx = tplbuffer->tupleCount;
    size_t len = left->size + right->size;

    tplbuffer->tuples[idx].columnCount = left->columnCount + right->columnCount;
    tplbuffer->tuples[idx].size        = len;

    // Fill identifiers for tuple
    for (size_t i = 0; i < tplbuffer->tuples[idx].columnCount; i++) {
        tplbuffer->tuples[idx].identifiers[i] = -1; // TODO: Any use?
    }
    // Add pointers to start of each column
    // Reuse the data in the original tuples
    // Move only pointers to two columns; do not copy data


    size_t i = 0;
    for (size_t j = 0; j < left->columnCount; j++) {
        tplbuffer->tuples[idx].pCols[i] = left->pCols[j];
        i++;
    }

    for (size_t j = 0; j < right->columnCount; j++) {
        tplbuffer->tuples[idx].pCols[i] = right->pCols[j];
        i++;
    }

    return &tplbuffer->tuples[idx];
}

Tuple* joinGetTuple(Operator* op) {

    if (
        op->info.join.left == NULL ||
        op->info.join.right == NULL
        ) {
        printf("Join left or right operator is NULL\n");
        exit(1);
    }
    

    if (
        op->info.join.left->type != OP_SCAN ||
        op->info.join.right->type != OP_SCAN
        ) {
        printf("Join left or right operator not of type OP_SCAN\n");
        printf("Left type: %d. Right type: %d.\n", op->info.join.left->type, op->info.join.right->type );
        exit(1);
    }

    /*
        This monstrosity collects the pointers to tuples
        in the right table/subquery into an array.
        The array is reused when iterating over values in
        the left table/subquery.
    */

    Tuple* rightTuple = NULL;
    
    if (op->info.join.rightTuplesCollected) {
        if (op->info.join.rightTupleIdx >= op->info.join.rightTupleCount) {
            op->info.join.rightTupleIdx = 0;
            op->info.join.lastTuple = NULL;
        }
        rightTuple = op->info.join.rightTuples[op->info.join.rightTupleIdx++];
        
    } else {
        rightTuple = op->info.join.right->getTuple(op->info.join.right);

        if (rightTuple == NULL) {
            op->info.join.rightTuplesCollected = true;
            op->info.join.lastTuple = NULL;
            op->info.join.rightTupleIdx = 0;
            rightTuple = op->info.join.rightTuples[op->info.join.rightTupleIdx++];
            
            
        } else {
            op->info.join.rightTuples[op->info.join.rightTupleIdx++] = rightTuple;
            op->info.join.rightTupleCount++;

        }
    }
    if (op->info.join.lastTuple == NULL) {
        op->info.join.lastTuple   = op->info.join.left->getTuple(op->info.join.left);
        if (op->info.join.lastTuple == NULL) {
            return NULL;
        }
    }

    return concat_tuples(op->info.join.lastTuple, rightTuple);
}
