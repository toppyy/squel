#include "../../include/executor/executor.h"

void printOp(Operator* op) {

    switch (op->type) {
        case OP_SCANTDB:
            printf("OP_SCANTDB");
            break;
        case OP_SCAN:
            printf("OP_SCAN");
            break;
        case OP_PROJECT:
            printf("OP_PROJECT");
            break;
        case OP_FILTER:
            printf("OP_FILTER");
            break;
        case OP_JOIN:
            printf("OP_JOIN");
            break;
        case OP_AGGREGATE:
            printf("OP_AGGREGATE");
            break;
        case OP_HASHJOIN:
            printf("OP_HASHJOIN");
            break;
        case OP_STMTCREATE:
            printf("OP_STMTCREATE");
            break;
        case OP_STMTINSERT:
            printf("OP_STMTINSERT");
            break;
        case OP_STMTEXPLAIN:
            break;
        default:
            printf("EXPLAIN-error: Unknown operation type");
            break;
    }

}

void explainOp(Operator* op) {

    if (!op) return;

    printOp(op);
    printf("\n");

    if (op->type == OP_FILTER) {
        explainOp(op->info.filter.next);
    }

    if (op->type == OP_JOIN || op->type == OP_HASHJOIN) {
        explainOp(op->info.join.filter);
        explainOp(op->info.join.left);
        explainOp(op->info.join.right);
    }

    if (op->child) {
        explainOp(op->child);
    }
}


void executeExplain(Operator* queryplan) {
    checkPtrNotNull(queryplan, "Execute-error: nullptr passed to executeExplain\n");
    
    /* Print the query plan */
    printf("******* EXPLAIN **********\n");
    explainOp(queryplan);
    printf("**************************\n");

}

