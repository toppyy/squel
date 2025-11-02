#include "../../include/planner/planner.h"

Operator* makeExplain(Node* node) {

    Operator* op = (Operator*) calloc(1, sizeof(Operator));
    op->type    = OP_STMTEXPLAIN;

    op->child = planQueryAst(node->next);

    return op;

}

