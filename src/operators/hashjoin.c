#include "../include/operators/hashjoin.h"



void hashjoinGetTuple(Operator* op, Tuple* tpl) {
    if (
        op->info.join.left == NULL ||
        op->info.join.right == NULL
        ) {
        printf("Join left or right operator is NULL\n");
        exit(1);
    }

    int joinColIdx     = op->info.join.filter->info.filter.boolExprList[2];
    int joinColOffset  = op->info.join.filter->resultDescription.pCols[joinColIdx];
    // int joinColOffset  = op->info.join.right->resultDescription.pCols[joinColIdx];
    
    if (!op->info.join.hashmap) {
        op->info.join.hashmap = initHashmap(1000); // TODO magic
        op->info.join.rightTuples = initTupleBuffer(JOINBUFFSIZE, TUPLESIZE);
    }


    Tuple* rightTuple = initTupleOfSize(TUPLESIZE);
    const char* joinValue;

    // This is only entered first time the operator is called
    while (!op->info.join.rightTuplesCollected) {

        rightTuple = getTupleFromBuffer(op->info.join.rightTuples);

        op->info.join.right->getTuple(op->info.join.right, rightTuple);
    
        if (isTupleEmpty(rightTuple)) {
            op->info.join.rightTuplesCollected = true;
            continue; 
        } 
        // Get value of join column
        joinValue = (const char*) getTupleCol(rightTuple, joinColOffset);

        insertToHashmap(op->info.join.hashmap, joinValue, op->info.join.rightTupleCount);

        op->info.join.rightTupleCount++;
    }



    // Nested join loop
    // For each tuple if left relation
    //      For each tuple in right relation
    //          if join_predicates(left,right) return tuple(left,right)

    if (op->info.join.leftTuple == NULL) {
        op->info.join.leftTuple = initTupleOfSize(TUPLESIZE);
    }

    if (isTupleEmpty(op->info.join.leftTuple)) {
        op->info.join.left->getTuple(op->info.join.left, op->info.join.leftTuple);
    }

    joinColIdx     = op->info.join.filter->info.filter.boolExprList[0];
    joinColOffset  = op->info.join.filter->resultDescription.pCols[joinColIdx];


    int tupleIdx;
    do {
        joinValue = (const char*) getTupleCol(op->info.join.leftTuple, joinColOffset);

        if (!isInHashmap(op->info.join.hashmap, joinValue)) {
            resetCursor(op->info.join.hashmap, joinValue);
            op->info.join.left->getTuple(op->info.join.left, op->info.join.leftTuple);
            continue;
        }


        tupleIdx = getValueFromHashmap(op->info.join.hashmap, joinValue);
        if (tupleIdx < 0) continue;

        rightTuple = getTupleByIndex(op->info.join.rightTuples, tupleIdx);

        // Create a new tuple by concating the tuples
        concatTuples(
            tpl,
            op->info.join.leftTuple,
            rightTuple,
            &op->info.join.left->resultDescription,
            &op->info.join.right->resultDescription
        );
            
        return;

    } while (!isTupleEmpty(op->info.join.leftTuple));
    
    // Join complete, we can free the buffer and the tuples associated
    freeTupleBuffer(op->info.join.rightTuples);
    freeTuple(op->info.join.leftTuple);
    markTupleAsEmpty(tpl);
    
}


