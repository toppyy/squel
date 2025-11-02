#include "../../include/planner/planner.h"

Operator* makeInsert(Node* node) {
    /*
        Expected tree for INSERT 
        INSERT -> ident_tbl -> SELECT
    */

    Operator* op = (Operator*) calloc(1, sizeof(Operator));
    op->type = OP_STMTINSERT;

    strcpy(op->info.insert.targetTableName, node->next->content);

    /* Plan the query */
    op->child = planQueryAst(node->next->next);

    return op;
}
