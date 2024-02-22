#include "../../include/planner/planner.h"

Operator* makeJoinOp(Node* node) {
    
        /*
            TODO:
                - Collect aliases for left and right
                - If not specified, make them up
                - pass alias + offset in result description to makeFilterOps
                - ON is expected to have aliases
        */

        Operator* opJoin = (Operator*) calloc(1, sizeof(Operator));
        opJoin->info.join.left     = makeScanOp(node);
        opJoin->info.join.right    = makeScanOp(node->next->next);
        opJoin->type = OP_JOIN;
        opJoin->info.join.rightTupleCount = 0;
        opJoin->info.join.rightTupleIdx = 0;
        opJoin->info.join.rightTuplesCollected = false;

        copyResultDescription(opJoin->info.join.left, opJoin,     0);
        copyResultDescription(opJoin->info.join.right, opJoin, opJoin->resultDescription.columnCount);

        /* ON-clause */
        Node* ON = node->next->next->next;
        Operator* opFilter = makeFilterOps(ON, opJoin);

        opJoin->info.join.filter = opFilter;

        return opJoin;


}