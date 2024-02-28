#include "../../include/planner/planner.h"

Operator* makeScanTDBOp(Node* node) {

    checkPtrNotNull(node,"Passed a NULL-pointer to makeScanTDBOp.");

    
    if (node->type != IDENT_TBL) {
        printf("Tried to make scan-operator from something else than a IDENT_TBL. Type: %d\n", node->type);
        exit(1);
    }



    char path[CHARMAXSIZE];
    char* ptr_path = path;

    buildPathToTDBtable(ptr_path, node->content);

    if (!fileExists(path)) {
        printf("Table '%s' does not exists in path %s\n", node->content, path);
        exit(1);
    }


    struct TDB tbldef = readTdbMetadata(path);


    Operator* op = (Operator*) calloc(1, sizeof(Operator));
    op->type = OP_SCANTDB;
    op->child                   = NULL;
    op->info.scan.tablefile     = NULL;
    op->info.scan.cursor        = 38;
    op->getTuple                = NULL;
    op->info.scan.tbldef        = tbldef;
    op->info.scan.recordSize    = 0;

    for (size_t i = 0; i < tbldef.colCount; i++) {        
        op->resultDescription.columns[i].type = tbldef.datatypes[i];
        strcpy(op->resultDescription.columns[i].name, tbldef.colNames[i]);
        strcpy(op->resultDescription.columns[i].resultSetAlias, node->alias);
        op->info.scan.recordSize += tbldef.lengths[i];
    }
    op->resultDescription.columnCount = tbldef.colCount;

    return op;
}
