#include "../include/operators/join.h"


void joinGetTuple(Operator* op, Tuple* tpl) {
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

        // Setup the tuple holding join results
        tpl->type    = TPL_JOIN;
        tpl->leftColumnCount = op->info.join.left->resultDescription.columnCount;

        // Init a buffer to hold the tuples from the right relation
        op->info.join.rightTuples = initTupleBuffer(JOINBUFFSIZE, TUPLESIZE);
    }

    Tuple* rightTuple;
    // This is only entered first time the operator is called
    while (!op->info.join.rightTuplesCollected) {

        rightTuple = getTupleFromBuffer(op->info.join.rightTuples);

        op->info.join.right->getTuple(op->info.join.right, rightTuple);
    
        if (isTupleEmpty(rightTuple)) {
            op->info.join.rightTuplesCollected = true;
            continue; 
        }

        op->info.join.rightTupleCount++;
    }


    // Nested join loop
    // For each tuple if left relation
    //      For each tuple in right relation
    //          if join_predicates(left,right) return tuple(left,right)

    if (op->info.join.leftTuple == NULL) {
        op->info.join.leftTuple = initTupleOfSize(TUPLESIZE);
        op->info.join.left->getTuple(op->info.join.left, op->info.join.leftTuple);
    }

    if (isTupleEmpty(op->info.join.leftTuple)) {
        op->info.join.left->getTuple(op->info.join.left, op->info.join.leftTuple);
    }

    do {
        
        if (op->info.join.rightTupleIdx >= op->info.join.rightTupleCount) {
            op->info.join.rightTupleIdx = 0;
            op->info.join.left->getTuple(op->info.join.left, op->info.join.leftTuple);
            if (isTupleEmpty(op->info.join.leftTuple)) {
                break;
            }

            continue;
        }

        rightTuple = getTupleByIndex(op->info.join.rightTuples, op->info.join.rightTupleIdx++);

        if (evaluateTuplesAgainstFilterOps(op->info.join.leftTuple, rightTuple, op->info.join.filter)) {
            tpl->left    = op->info.join.leftTuple;
            tpl->right   = rightTuple;
            return;
        }
    } while(!isTupleEmpty(op->info.join.leftTuple));
    
    // Join complete, we can free the buffer and the tuples associated
    freeTupleBuffer(op->info.join.rightTuples);
    freeTuple(op->info.join.leftTuple);
    markTupleAsEmpty(tpl);
    
}


