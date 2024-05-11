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
        opJoin->iteratorTupleOffset = -1;
        opJoin->info.join.filterTupleOffset = -1;


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
        Operator* opFilter = makeFilterOps(ON, opJoin);
        opJoin->info.join.filter = opFilter;

        for (size_t i = 0; i < opJoin->resultDescription.columnCount; i++) {
                opFilter->resultDescription.pCols[i] = opJoin->resultDescription.pCols[i];
        }


        /* Filter function list */
        Operator* tmp = makeFilterFunction(ON->child, opJoin);
        free(tmp);

        return opJoin;
}