#include "../../include/executor/executor.h"

struct TDB* tbldef = NULL;
size_t tupleSize = 0;
FILE* f = NULL;


void handleTupleInsert(Tuple* tpl) {

    if (f == NULL) {
        printf("No file to insert to\n");
        exit(1);
    }

    char* ptr = NULL;
    void* diskBuffer = calloc(1, tupleSize);
    void* diskBufferCursor = diskBuffer;
    int tmp = 0;
    size_t strLen = 0;
    for (size_t i = 0; i < tbldef->colCount; i++) {

        ptr = getCol(tpl,i);

        if (tbldef->datatypes[i] == DTYPE_INT) {
            tmp = atoi(ptr);
            memcpy(diskBufferCursor, &tmp, sizeof(tmp));
            diskBufferCursor += sizeof(tmp);
            continue;
        }

        if (tbldef->datatypes[i] == DTYPE_STR) {
            strLen = strlen(ptr) + 1;
            if (strLen > (size_t) tbldef->lengths[i]) {
                printf("String data is truncated due to column size\n");
                strLen = tbldef->lengths[i];
            }
            memcpy(diskBufferCursor, ptr, strLen);
            diskBufferCursor += strLen;
            // A string has a specific size. Pad the string with '\0' if necessary
            memset(diskBufferCursor, 0, tbldef->lengths[i] - strLen);
            diskBufferCursor += tbldef->lengths[i] - strLen;
            continue;
        }

        printf("Don't know how to write type %d to disk\n", tbldef->datatypes[i]);
        exit(1);

    }
    size_t bytesWritten = fwrite(diskBuffer, tupleSize, 1, f);
    assert(bytesWritten > 0);
    free(diskBuffer);
}




void executeInsert(Node* node) {
    /*
        Expected tree for INSERT 
        INSERT -> ident_tbl -> SELECT
    */

    char filepath[CHARMAXSIZE];
    char* ptr_filepath = filepath;
    buildPathToTDBtable(ptr_filepath, node->next->content);

    struct TDB tbl = readTdbMetadata(ptr_filepath);

    /* Plan the query */
    Operator* queryplan = planQuery(node->next->next);

    /* Check target table matches result description of the query */
    // printf("Tbl has %ld columns, query has %ld columns\n", tbl.colCount, queryplan->resultDescription.columnCount);
    assert(queryplan->resultDescription.columnCount == tbl.colCount);

    for (size_t i = 0; i < tbl.colCount; i++) {
        if (queryplan->resultDescription.columns[i].type != tbl.datatypes[i]) {
            printf("Target table and query columns do not match at index %ld\n", i);
            printf("Target: %d, query: %d\n",tbl.datatypes[i], queryplan->resultDescription.columns[i].type);
            exit(1);
        }
    }

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
    execute(queryplan, false, handleTupleInsert);

    /* Clean up */
    fclose(f);
    free(queryplan);
}

