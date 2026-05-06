#include "../../include/planner/planner.h"


Operator* makeJoinFilterOps(
    Node* where_node,
    Operator* join_op,
    ResultSet leftResult,
    ResultSet rightResult __attribute__((unused))
) {
    // Make regular filter ops (ie. where agains one table)
    Operator* filterOps = makeFilterOps(where_node, join_op);    
    
    // Alter so that indexes point to offsets in left and right tables
    // Otherwise would have to create a new tuple that has both columns

    Operator* curOp = filterOps;
    while (curOp != NULL) {

        if ((size_t) curOp->info.filter.boolExprList[2] > leftResult.columnCount) {
            curOp->info.filter.boolExprList[2] -= leftResult.columnCount;
        }
        if ((size_t) curOp->info.filter.boolExprList[0] > leftResult.columnCount) {
            curOp->info.filter.boolExprList[0] -= leftResult.columnCount;
        }

        curOp = curOp->info.filter.next;    
    }
    

    // printf(
    //     "%d vs. %d (%d). Expected 1 vs 1\n"
    //     ,filterOps->info.filter.boolExprList[0]
    //     ,filterOps->info.filter.boolExprList[2]
    //     ,filterOps->info.filter.boolExprList[1]
    // );
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

    // -1 signifies equality (see filter.c)
    if (filterOp->info.filter.boolExprList[1] != -1) {
        return OP_JOIN;
    }

    // Hashjoin disabled
    if (getOption(OPT_ENABLE_HJ) == 0) {
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
    opJoin->resultDescription.id = 123;


    copyResultDescription(opJoin->info.join.left , opJoin, 0);
    copyResultDescription(opJoin->info.join.right, opJoin, opJoin->resultDescription.columnCount);

    opJoin->resultDescription.columnCount = left->resultDescription.columnCount + right->resultDescription.columnCount;
    opJoin->resultDescription.size        = left->resultDescription.size        + right->resultDescription.size;

    // Copy data
    if (
            (left->resultDescription.size + right->resultDescription.size) > JOINTUPLESIZE
    ) {
            printf("Tried to concat tuples from join but their size (%ld) exceeds %d\n", left->resultDescription.size + right->resultDescription.size, JOINTUPLESIZE);
            exit(1);
    }


    /* ON-clause */
    Operator* opFilter = makeJoinFilterOps(ON, opJoin, left->resultDescription, right->resultDescription);
    opJoin->info.join.filter = opFilter;


    opJoin->type = deduceJoinType(opFilter);


    return opJoin;
}