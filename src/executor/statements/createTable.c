#include "../../include/executor/executor.h"



TDB constructTDB(CreateInfo info) {


    struct TDB tbl;

    for (size_t colIdx = 0; colIdx < info.colCount; colIdx++) {

        strcpy(tbl.colNames[colIdx], info.columns[colIdx].name);
        tbl.datatypes[colIdx] = info.columns[colIdx].type;
        tbl.lengths[colIdx] = sizeof(long);

        if (tbl.datatypes[colIdx] == DTYPE_STR) {
            tbl.lengths[colIdx] = info.columns[colIdx].size;
        }

    }

    tbl.colCount = info.colCount;
    tbl.records = 0;

    return tbl;
}


void executeCreateTable(Operator* op) {

    checkPtrNotNull(op, "Execute-error: nulltpr passed to executeCreateTable\n");


    struct TDB tbl = constructTDB(op->info.create);
    strcpy(tbl.alias, op->info.create.objectName);

    char filepath[CHARMAXSIZE];
    char* ptr_filepath = filepath;
    buildPathToTDBtable(ptr_filepath, tbl.alias);

    writeTdbMetadata(filepath, tbl);
}

