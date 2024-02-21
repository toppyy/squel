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
            // NOTE: Indexed to 0. Multiple aggregations not supported ATM
            op->resultDescription.columns[0].type = DTYPE_LONG;
            if (node->child != NULL) {
                strcpy(op->resultDescription.columns[0].name, node->child->content);
            }
            break;
        }

        i++;
        node = node->next;        
        if (node == NULL) {
            printf("makeAggregateOp: No node with aggregation function passed.\n");
            exit(1);
        }
    };

    // Find the index of the column referred to in the function call if it's
    // a column reference
    op->info.aggregate.colToAggregate = -1;
    if (node->child->type == IDENT_COL) {
        bool matched = false;
        for (size_t j = 0; j < child_op->resultDescription.columnCount; j++) {
        
            if (
                strcmp(op->resultDescription.columns[0].name, child_op->resultDescription.columns[j].name) == 0
            ) {
                op->info.aggregate.colToAggregate = j;
                op->resultDescription.columnCount++;
                matched = true;
                break;
            }

        }
        if (!matched) {
            printf("Could not find column '%s'\n", op->resultDescription.columns[0].name);
            exit(1);
        }
    } else {
        printf("Aggregates on constants not supported\n");
        exit(1);
    }


    return op;
}
