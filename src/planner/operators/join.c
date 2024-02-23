#include "../../include/planner/planner.h"

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
        opJoin->type = OP_JOIN;
        opJoin->info.join.rightTupleCount = 0;
        opJoin->info.join.rightTupleIdx = 0;
        opJoin->info.join.rightTuplesCollected = false;

        copyResultDescription(opJoin->info.join.left, opJoin,     0);
        copyResultDescription(opJoin->info.join.right, opJoin, opJoin->resultDescription.columnCount);

        /* ON-clause */
        Operator* opFilter = makeFilterOps(ON, opJoin);
        opJoin->info.join.filter = opFilter;

        return opJoin;


}