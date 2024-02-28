#include "../include/executor/executor.h"


void executeStatement(Node* node) {
    checkPtrNotNull(node, "NULL node passed to executeStatement");
    checkPtrNotNull(node->next, "Node passed to executeStatement has no next");

    switch (node->type) {
        case STMTCREATE:
            if (node->next->type != TABLE) {
                printf("Don't know how to create anything else than a table.\n");
                exit(1);
            }
            executeCreateTable(node);
            break;
        case STMTINSERT:
            executeInsert(node);
            break;
        default:
            printf("Don't know how execute statement of type %d\n", node->type);
            exit(1);
    }
}