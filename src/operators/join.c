#include "../include/operators/join.h"


void concatTuples(Tuple* returnTpl, Tuple* leftTpl, Tuple* rightTpl, ResultSet* left, ResultSet* right) {

    if (
        left == NULL ||
        right == NULL
    ) {
        printf("Passed a NULL pointer to concatTuples\n");
        exit(1);
    }

    returnTpl->type = leftTpl->type; // TODO: what if these differ between left and right?

    memcpy(returnTpl->data, leftTpl->data, leftTpl->size);
    memcpy(returnTpl->data + left->size, rightTpl->data, rightTpl->size);



    for (size_t i = 0; i < left->columnCount; i++) {
        returnTpl->offsets[i]   = leftTpl->offsets[i];
        returnTpl->sizes[i]     = leftTpl->sizes[i];
        returnTpl->casted[i]    = leftTpl->casted[i];
        returnTpl->longs[i]     = leftTpl->longs[i];
    }

    returnTpl->longCount = leftTpl->longCount;
    
    for (size_t i = 0; i < right->columnCount; i++) {

        if (
            // For delimited tables, the offsets refer to offsets into the longs-array
            // if the value has been casted
            (rightTpl->casted[i]) & (returnTpl->type == TPL_DELIMITED)
        ) {
            returnTpl->offsets[i + left->columnCount]   =  leftTpl->longCount + rightTpl->offsets[i];
        } else {
            returnTpl->offsets[i + left->columnCount]   =  leftTpl->size + rightTpl->offsets[i];
        }

        returnTpl->sizes[i + left->columnCount]     =  rightTpl->sizes[i];
        returnTpl->casted[i + left->columnCount]    =  rightTpl->casted[i];
        returnTpl->longs[i + leftTpl->longCount]    =  rightTpl->longs[i];
    }

    returnTpl->longCount = leftTpl->longCount + rightTpl->longCount;
    returnTpl->size = leftTpl->size + rightTpl->size;

    // printf("concat: '");
    // for (size_t i = 0; i < returnTpl->size; i++) {
    //     char c = ((char*) returnTpl->data)[i];
    //     if (c == '\0') c = '|';
    //     printf("%c", c);
    // }
    // printf("'\n");


 

}

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
            // Create a new tuple by concating the tuples
            concatTuples(
                tpl,
                op->info.join.leftTuple,
                rightTuple,
                &op->info.join.left->resultDescription,
                &op->info.join.right->resultDescription
            );
            
            return;
        }
    } while(!isTupleEmpty(op->info.join.leftTuple));
    
    // Join complete, we can free the buffer and the tuples associated
    freeTupleBuffer(op->info.join.rightTuples);
    freeTuple(op->info.join.leftTuple);
    markTupleAsEmpty(tpl);
    
}


