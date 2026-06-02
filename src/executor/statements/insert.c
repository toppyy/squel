#include "../../include/executor/executor.h"

struct TDB* tbldef = NULL;
size_t tupleSize = 0;
void* insertBuffer = NULL;
FILE* f = NULL;


void handleTupleInsert(Operator* op __attribute__((unused)), Tuple* tpl __attribute__((unused))) {

    if (f == NULL) {
        printf("No file to insert to\n");
        exit(1);
    }

    memset(insertBuffer, 0, TDBRECORDMAXSIZE);

    tupleSize = 0;
    long num;

    for (size_t i = 0; i < op->resultDescription.columnCount; i++) {

        if (op->resultDescription.columns[i].type == DTYPE_LONG) {
            num = getTupleLongColByIndex(tpl, i);
            memcpy((char*) insertBuffer + tupleSize, &num, sizeof num);
            tupleSize += sizeof(num);
            continue;
        }
        
        if (op->resultDescription.columns[i].type == DTYPE_STR) {
            memcpy(insertBuffer + tupleSize, tpl->data + tpl->offsets[i], TDBMAXSTRINGSIZE);
            tupleSize += TDBMAXSTRINGSIZE;
            continue;
        }
        
        assert(0);
    }

    size_t bytesWritten = fwrite(insertBuffer, tupleSize, 1, f);
    assert(bytesWritten > 0);
}


void executeInsert(Operator* insertOp) {

    checkPtrNotNull(insertOp, "Execute-error: nulltpr passed to executeInsert\n");

    char filepath[CHARMAXSIZE];
    char* ptr_filepath = filepath;
    buildPathToTDBtable(ptr_filepath, insertOp->info.insert.targetTableName);

    struct TDB tbl = readTdbMetadata(ptr_filepath);

    Operator* op = insertOp->child;

    /* Check target table matches result description of the query */
    assert(op->resultDescription.columnOrderCount == tbl.colCount);

    // We need another crappy index as all of the columns in the operator
    // may not be active
    size_t colIdx = 0;
    
    for (size_t i = 0; i < op->resultDescription.columnCount; i++) {

        if (!op->resultDescription.columns[i].active) continue;

        if (op->resultDescription.columns[i].type != tbl.datatypes[colIdx]) {
            printf("Target table and query columns do not match at index %ld\n", i);
            printf("Target: %d, query: %d\n",tbl.datatypes[colIdx], op->resultDescription.columns[i].type);
            exit(1);
        }
        colIdx++;
    }

    /*  Allocate memory for the insert buffer.
        We need parse the strings into numeric types and need memory for that.
    */

    insertBuffer = malloc(TDBRECORDMAXSIZE);


    /* Prep for writing tuples to disk */

    tbldef = &tbl;
    tupleSize  = 0;
    for (size_t i = 0; i < tbl.colCount; i++) {
        tupleSize += tbl.lengths[i];
    }
    
    f = fopen(ptr_filepath, "w+");
    if (f == NULL) {
        printf("Unable to open the file at path '%s'\n", ptr_filepath);
        exit(1);
    }

    writeTdbMetadataToFD(f, tbl);

    /* Execute the query */
    execute(op, handleTupleInsert);

    /* Clean up */
    free(insertBuffer);
    fclose(f);
}

