#include "../include/operators/join.h"


Tuple* concat_tuples(Tuple* left, Tuple* right) {

    if (
        left == NULL ||
        right == NULL
    ) {
        printf("Passed a NULL pointer to concat_tuples\n");
        exit(1);
    }

    
    size_t len = left->size + right->size;

    Tuple* tpl = addTuple();
    tpl->columnCount = left->columnCount + right->columnCount;
    tpl->size        = len;

    // Copy data

    memcpy(tpl->data, left->data, left->size);
    memcpy(tpl->data +  left->size + 1, right->data, right->size);

    // Add pointers to start of each column
    // Reuse the data in the original tuples
    // Move only pointers to two columns; do not copy data

    size_t i = 0;
    for (size_t j = 0; j < left->columnCount; j++) {
        tpl->pCols[i] = left->pCols[j];
        i++;
    }

    size_t offset = tpl->pCols[i-1] + strlen(getCol(left, i-1)) + 1;
    for (size_t j = 0; j < right->columnCount; j++) {
        tpl->pCols[i] = offset + right->pCols[j];
        i++;
    }


    return tpl;
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
    Tuple* tpl = NULL;

    do {
        
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
        tpl = concat_tuples(op->info.join.lastTuple, rightTuple);
        if (evaluateTupleAgainstFilterOps(tpl, op->info.join.filter)) {
            return tpl;
        }
    } while(true);

}
