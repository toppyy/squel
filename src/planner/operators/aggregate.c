#include "../../include/planner/planner.h"


AggregationType mapFunctionNameToAggregation(char* name) {
    if (strcmp(name, "COUNT") == 0) {
        return COUNT;
    }
    if (strcmp(name, "AVG") == 0) {
        return AVG;
    }
    if (strcmp(name, "SUM") == 0) {
        return SUM;
    }

    printf("Unknown aggregation function '%s'\n", name);
    exit(1);
}

Operator* makeAggregateOp(Node* node, Operator* child_op) {

    if (child_op == NULL) {
        printf("Passed a NULL-pointer as child to makeAggregateOp\n");
        exit(1);
    }

    Operator* op = (Operator*) calloc(1, sizeof(Operator));
    op->type    = OP_AGGREGATE;
    op->child   = NULL;

    op->info.aggregate.aggregationDone = false;


    /* GROUP BY not implement, so columns not included in a function call are ignored */
    
    int i = 0;

    for (;;) {
        if (node->type == IDENT_FUN) {
            op->info.aggregate.aggtype = mapFunctionNameToAggregation(node->content);
            op->resultDescription.columns[0].type = DTYPE_LONG;
            strcpy(op->resultDescription.columns[0].name, node->content); 
            break;
        }

        i++;
        node = node->next;        
        if (node == NULL) {
            printf("makeAggregateOp: No node with aggregation function passed.\n");
            exit(1);
        }
    };


    return op;
}
