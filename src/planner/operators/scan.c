#include "../../include/planner/planner.h"

Operator* makeScanOp(Node* node) {

    checkPtrNotNull(node,"Passed a NULL-pointer to makeScanOp.");

    
    if (node->type != FILEPATH) {
        printf("Tried to make scan-operator from something else than a FILEPATH. Type: %d\n", node->type);
        exit(1);
    }

    TableMetadata tbl;
    memset(&tbl, 0, sizeof(tbl));
    catalogFile(node->content, &tbl, DELIMITER);

    Operator* op = (Operator*) calloc(1, sizeof(Operator));
    op->type = OP_SCAN;
    memcpy(&op->info.scan.table, &tbl, sizeof(tbl));
    op->child                   = NULL;
    op->info.scan.tablefile     = NULL;
    op->info.scan.cursor        = 0;
    op->getTuple                = NULL;

    for (size_t i = 0; i < tbl.columnCount; i++) {        
        op->resultDescription.columns[i].type = tbl.columns[i].type;
        op->resultDescription.columns[i].identifier = tbl.columns[i].identifier;
        strcpy(op->resultDescription.columns[i].name, tbl.columns[i].name);
        strcpy(op->resultDescription.columns[i].resultSetAlias, node->alias);
    }
    op->resultDescription.columnCount = tbl.columnCount;

    return op;
}
