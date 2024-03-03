#include "../include/operators/join.h"


int concatTuples(int tupleOffset,int leftOffset,int rightOffset, ResultSet* left, ResultSet* right) {

    if (
        left == NULL ||
        right == NULL
    ) {
        printf("Passed a NULL pointer to concatTuples\n");
        exit(1);
    }

    void* address = getTuple(tupleOffset);

    memset(address, 0, left->size + right->size);
    memcpy(address, getTuple(leftOffset), left->size);
    memcpy(address + left->size, getTuple(rightOffset), right->size);

    return tupleOffset;
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

        We store one of the tables in the join in memory.
        Which is why the tuples from the right table are copied 
        to the buffer pool. Their original location will be
        rewritten by child operators iterating over tuples.
        

    */


    int rightTupleOffset = 0, originalOffset;
    // Reuse this and only create a new tuple if it passes the filter
    int offset = 0;
     
    // Reserve space from the buffer pool so that we can concatenate tuples
    int filterTupleOffset = getCurrentOffset();
    reserveSpaceBufferpool(filterTupleOffset, JOINTUPLESIZE);

    // This is only entered first time the operator is called
    while (!op->info.join.rightTuplesCollected) {
        
        originalOffset = op->info.join.right->getTuple(op->info.join.right);
    
        if (originalOffset == -1) {
            op->info.join.rightTuplesCollected = true;
            op->info.join.lastTupleOffset = -1;
            op->info.join.rightTupleIdx = 0;            
            continue;  
        } 

        rightTupleOffset = addToBufferPoolFromOffset(originalOffset, op->info.join.right->resultDescription.size);

        op->info.join.rightTuples[op->info.join.rightTupleIdx++] = rightTupleOffset;
        op->info.join.rightTupleCount++;
    }

    // Join loop
    do {
        
        if (op->info.join.rightTupleIdx >= op->info.join.rightTupleCount) {
            op->info.join.rightTupleIdx = 0;
            op->info.join.lastTupleOffset = -1;
        }
        
        if (op->info.join.lastTupleOffset == -1) {
            offset = op->info.join.left->getTuple(op->info.join.left);
            if (offset == -1) {
                return -1;
            }
            op->info.join.lastTupleOffset = offset;
        }

        rightTupleOffset = op->info.join.rightTuples[op->info.join.rightTupleIdx++];

        /*
            We must get the tuples again 'cause the data-pointers might be corrupted. 
            Getting them from the buffer pool updates the pointers.
        */


        offset = concatTuples(
            filterTupleOffset,
            op->info.join.lastTupleOffset,
            rightTupleOffset,
            &op->info.join.left->resultDescription,
            &op->info.join.right->resultDescription
        );

        if (evaluateTupleAgainstFilterOps(offset, op->info.join.filter)) {
            
            int newTupleOffset = addToBufferPool(getTuple(offset), op->resultDescription.size);
            return newTupleOffset;
        }
    } while(true);

}
