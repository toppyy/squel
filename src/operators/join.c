#include "../include/operators/join.h"


Tuple* concatTuples(Tuple* tpl, Tuple* left, Tuple* right) {

    if (
        left == NULL ||
        right == NULL
    ) {
        printf("Passed a NULL pointer to concatTuples\n");
        exit(1);
    }
    
    tpl->columnCount = left->columnCount + right->columnCount;
    tpl->size        = left->size + right->size;

    // Copy data
    if (
        (left->size + right->size) > JOINTUPLESIZE
    ) {
        printf("Tried to concat tuples from join but their size (%ld) exceeds %d\n", left->size + right->size, JOINTUPLESIZE);
        exit(1);
    }
    memset(tpl->data, 0, left->size + right->size);
    memcpy(tpl->data, left->data, left->size);
    memcpy(tpl->data + left->size, right->data, right->size);

    // Add pointers to start of each column
    // Reuse the data in the original tuples
    // Move only pointers to two columns; do not copy data

    size_t i = 0;
    for (size_t j = 0; j < left->columnCount; j++) {
        tpl->pCols[i] = left->pCols[j];
        // printf("Pcol %ld is '%ld'\n", i, tpl->pCols[i]);
        i++;
    }

    size_t offset = left->size;

    for (size_t j = 0; j < right->columnCount; j++) {
        tpl->pCols[i] = offset + right->pCols[j];
        // printf("Pcol %ld is %ld\n", i, tpl->pCols[i]);
        i++;
    }

    // printf("Tuple-size is %ld\n", tpl->size);

    return tpl;
}

int joinGetTuple(Operator* op) {

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
    Tuple* filterTuple = addTuple(); // Reuse this and only create a new tuple if it passes the filter
    
    // Reserve space from the buffer pool so that we can concatenate tuples
    reserveSpaceBufferpool(filterTuple->data, JOINTUPLESIZE);

    // This is only entered first time the operator is called
    while (!op->info.join.rightTuplesCollected) {
        
        rightTuple = getTuple(op->info.join.right->getTuple(op->info.join.right));

        if (rightTuple == NULL) {
            op->info.join.rightTuplesCollected = true;
            op->info.join.lastTuple = NULL;
            op->info.join.rightTupleIdx = 0;            
            
        } else {
            op->info.join.rightTuples[op->info.join.rightTupleIdx++] = rightTuple->idx;
            op->info.join.rightTupleCount++;
        }
    }

    // Join loop
    do {
        
        if (op->info.join.rightTupleIdx >= op->info.join.rightTupleCount) {
            op->info.join.rightTupleIdx = 0;
            op->info.join.lastTuple = NULL;
        }
        
        if (op->info.join.lastTuple == NULL) {
            op->info.join.lastTuple   = getTuple(op->info.join.left->getTuple(op->info.join.left));
            if (op->info.join.lastTuple == NULL) {
                return -1;
            }
        }

        rightTuple = getTuple(op->info.join.rightTuples[op->info.join.rightTupleIdx++]);

        /*
            We must get the tuples again 'cause the data-pointers might be corrupted. 
            Getting them from the buffer pool updates the pointers.
        */
        tpl = concatTuples(getTuple(filterTuple->idx), getTuple(op->info.join.lastTuple->idx), rightTuple);

        if (evaluateTupleAgainstFilterOps(tpl, op->info.join.filter)) {
            Tuple* newTuple = addTuple();
            memcpy(newTuple, filterTuple, sizeof(*filterTuple));
            return newTuple->idx;
        }
    } while(true);

}
