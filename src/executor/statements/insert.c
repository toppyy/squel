#include "../../include/executor/executor.h"

struct TDB* tbldef = NULL;
size_t tupleSize = 0;
void* insertBuffer = NULL;
FILE* f = NULL;

size_t rowCounter = 0;


void handleTupleInsert(Operator* op, Tuple* tpl) {

    if (f == NULL) {
        printf("No file to insert to\n");
        exit(1);
    }

    memset(insertBuffer, 0, TDBRECORDMAXSIZE);

    tupleSize = 0;
    size_t i = 0;
    long num;


    for (size_t idx = 0; idx < op->resultDescription.columnOrderCount; idx++) {
        
        if (!op->resultDescription.columns[idx].active) continue;
        i = op->resultDescription.columnOrder[idx];

        if (op->resultDescription.columns[i].type == DTYPE_LONG) {
            num = getTupleLongColByIndex(tpl, i);
            memcpy((char*) insertBuffer + tupleSize, &num, sizeof num);
            tupleSize += sizeof(num);

            // maintain stats
            void* ptr = op->colStats[i].max + (sizeof(num) * op->rowGroups);
            if (num > *((long*) ptr)) {
                memcpy(ptr, &num, sizeof num); 
            }

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

void handleTupleInsertStatistics(Operator* op, Tuple* tpl) {
    rowCounter++;
    if (rowCounter >= getOption(OPT_TDB_ROWGROUP_SIZE)) {
        op->rowGroups++;
        rowCounter = 0;
        printf("rowGroups: %ld\n", op->rowGroups);
    }
    handleTupleInsert(op, tpl);
}


void executeInsert(Operator* insertOp) {

    checkPtrNotNull(insertOp, "Execute-error: nulltpr passed to executeInsert\n");

    char filepath[CHARMAXSIZE];
    char* ptr_filepath = filepath;
    buildPathToTDBtable(ptr_filepath, insertOp->info.insert.targetTableName);

    struct TDB tbl = readTdbMetadata(ptr_filepath);

    Operator* op = insertOp->child;

    /* Check target table matches result description of the query */

    printf("op->resultDescription.columnOrderCount: %ld, tbl.colCount: %ld\n", op->resultDescription.columnOrderCount, tbl.colCount);
    assert(op->resultDescription.columnOrderCount == tbl.colCount);

    // We need another crappy index as all of the columns in the operator
    // may not be active
    
    for (size_t i = 0; i < op->resultDescription.columnOrderCount; i++) {
        size_t colIdx = op->resultDescription.columnOrder[i];
        if (!op->resultDescription.columns[colIdx].active) continue;

        if (op->resultDescription.columns[colIdx].type != tbl.datatypes[colIdx]) {
            printf("Target table and query columns do not match at index %ld\n", i);
            printf("Target: %d, query: %d\n",tbl.datatypes[colIdx], op->resultDescription.columns[colIdx].type);
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
    execute(op, handleTupleInsertStatistics);

    /* Write footer */
    printf("op->rowGroups: %ld\n", op->rowGroups);
    for (size_t rg = 0; rg < op->rowGroups; rg++) {
        for (size_t i = 0; i < op->resultDescription.columnOrderCount; i++) {

            size_t colIdx = op->resultDescription.columnOrder[i];
            if (!op->resultDescription.columns[colIdx].active) continue;
            printf("Rowgroup %ld has column %ld with max value of %ld\n", rg, colIdx, *((long*) (op->colStats[colIdx].max + (rg * sizeof(long)))));
        }
    }

    /* Clean up */
    free(insertBuffer);
    fclose(f);
}

