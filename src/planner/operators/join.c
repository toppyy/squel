#include "../../include/planner/planner.h"


Operator* makeJoinFilterOps(
    Node* where_node,
    Operator* join_op,
    ResultSet leftResult,
    ResultSet rightResult
) {
    // Make regular filter ops (ie. where agains one table)
    Operator* filterOps = makeFilterOps(where_node, join_op);

    // makeFilterOps uses the result description of join_op
    // However, when filtering we want to use the original pointers to columns
    // so we must restore them from result descriptors

    size_t offset = 0;
 
    for (size_t i = 0; i < leftResult.columnCount; i++) {
        filterOps->resultDescription.pCols[i] = leftResult.pCols[i];
        offset++;
    }
    for (size_t i = 0; i < rightResult.columnCount; i++) {
        filterOps->resultDescription.pCols[i + offset] = rightResult.pCols[i];
    }


    // Alter so that indexes point to offsets in left and right tables
    // Otherwise would have to create a new tuple that has both columns

    Operator* curOp = filterOps;
    while (curOp != NULL) {
        if ((size_t) curOp->info.filter.boolExprList[2] > leftResult.columnCount) {
            curOp->info.filter.boolExprList[2] -= leftResult.columnCount;
        }

        curOp = curOp->info.filter.next;    
    }

    return filterOps;
}

OperatorType deduceJoinType(Operator* filterOp) {
    // Atm we can do a hash join
    // if and only if:
    //  - There's only one join condition
    //  - The condition is an equality comparison

    if (filterOp->info.filter.next) {
        return OP_JOIN;
    }

    if (filterOp->info.filter.boolExprListSize < 3) {
        return OP_JOIN;
    }

    if (filterOp->info.filter.boolExprList[1] != -1) {
        return OP_JOIN;
    }

    return OP_HASHJOIN;

}


Operator* makeJoinOp(Operator* left, Operator* right, Node* ON) {
    
        /*
            TODO:
                - Collect aliases for left and right
                - If not specified, make them up
                - pass alias + offset in result description to makeFilterOps
                - ON is expected to have aliases
        */

        Operator* opJoin = (Operator*) calloc(1, sizeof(Operator));
        opJoin->info.join.left     = left;
        opJoin->info.join.right    = right;
        opJoin->info.join.rightTupleCount = 0;
        opJoin->info.join.rightTupleIdx = 0;
        opJoin->info.join.rightTuplesCollected = false;
        opJoin->iteratorTupleOffset = -1;


        copyResultDescription(opJoin->info.join.left, opJoin,     0);
        copyResultDescription(opJoin->info.join.right, opJoin, opJoin->resultDescription.columnCount);

        opJoin->resultDescription.columnCount = left->resultDescription.columnCount + right->resultDescription.columnCount;
        opJoin->resultDescription.size        = left->resultDescription.size + right->resultDescription.size;

        // Copy data
        if (
                (left->resultDescription.size + right->resultDescription.size) > JOINTUPLESIZE
        ) {
                printf("Tried to concat tuples from join but their size (%ld) exceeds %d\n", left->resultDescription.size + right->resultDescription.size, JOINTUPLESIZE);
                exit(1);
        }


        /* Copy column start positions */
        size_t i = 0;
        for (size_t j = 0; j < left->resultDescription.columnCount; j++) {
                opJoin->resultDescription.pCols[i] = left->resultDescription.pCols[j];
                i++;
        }

        size_t offset = left->resultDescription.size;

        for (size_t j = 0; j < right->resultDescription.columnCount; j++) {
                opJoin->resultDescription.pCols[i] = offset + right->resultDescription.pCols[j];
                i++;
        }

        /* ON-clause */
        Operator* opFilter = makeJoinFilterOps(ON, opJoin, left->resultDescription, right->resultDescription);
        opJoin->info.join.filter = opFilter;


        opJoin->type = deduceJoinType(opFilter);


        return opJoin;
}