#include "../include/operators/join.h"


void concatTuples(Tuple* returnTpl, Tuple* leftTpl, Tuple* rightTpl, ResultSet* left, ResultSet* right) {

    if (
        left == NULL ||
        right == NULL
    ) {
        printf("Passed a NULL pointer to concatTuples\n");
        exit(1);
    }

    void* address = calloc(1, left->size + right->size);
    memcpy(address, leftTpl->data, left->size);
    memcpy(address + left->size, rightTpl->data, right->size);
    returnTpl->data = address;
}

Tuple* joinGetTuple(Operator* op) {

    if (
        op->info.join.left == NULL ||
        op->info.join.right == NULL
        ) {
        printf("Join left or right operator is NULL\n");
        exit(1);
    }
    
    /*
        This monstrosity collects the pointers to tuples
        in the right table/subquery into an array.
        The array is reused when iterating over values in
        the left table/subquery.

        We store one of the tables in the join in memory.
        Which is why the tuples from the right table are copied 
        to a buffer.
    */
    
    if (!op->info.join.rightTuples) {
        op->info.join.rightTuples = initTupleBuffer(JOINPTRBUFFER);
    }

    Tuple* rightTuple;
    // This is only entered first time the operator is called
    while (!op->info.join.rightTuplesCollected) {
        
        rightTuple = op->info.join.right->getTuple(op->info.join.right);
    
        if (rightTuple == NULL) {
            op->info.join.rightTuplesCollected = true;
            continue; 
        } 

        addTupleToBuffer(rightTuple, op->info.join.rightTuples);
        op->info.join.rightTupleCount++;

        if (op->info.join.rightTupleCount >= JOINPTRBUFFER) {
            printf("Can't fit the right table in the query into joinbuffer. Increase JOINPTRBUFFER\n");
            exit(1);
        }
    }

    // Nested join loop
    // For each tuple if left relation
    //      For each tuple in right relation
    //          if join_predicates(left,right) return tuple(left,right)

    if (op->info.join.leftTuple == NULL) {
        op->info.join.leftTuple = op->info.join.left->getTuple(op->info.join.left);
    }

    Tuple* leftTuple = op->info.join.leftTuple; 

    do {
        
        if (op->info.join.rightTupleIdx >= op->info.join.rightTupleCount) {
            op->info.join.rightTupleIdx = 0;
            freeTuple(leftTuple);
            op->info.join.leftTuple = op->info.join.left->getTuple(op->info.join.left);
            leftTuple = op->info.join.leftTuple;
            continue;
        }

        rightTuple = getTupleByIndex(op->info.join.rightTuples, op->info.join.rightTupleIdx++);

        if (evaluateTuplesAgainstFilterOps(leftTuple, rightTuple, op->info.join.filter)) {

            Tuple* newTuple = initTuple();
            // Create a new tuple by concating the tuples
            concatTuples(
                newTuple,
                leftTuple,
                rightTuple,
                &op->info.join.left->resultDescription,
                &op->info.join.right->resultDescription
            );
            return newTuple;
        }
    } while(leftTuple != NULL);
    
    // Join complete, we can free the buffer and the tuples associated
    freeTupleBuffer(op->info.join.rightTuples);
    return NULL;
}

