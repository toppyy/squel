#include "../../include/executor/executor.h"

struct TDB* tbldef = NULL;
size_t tupleSize = 0;
FILE* f = NULL;


void handleTupleInsert(Tuple* tpl) {

    if (f == NULL) {
        printf("No file to insert to\n");
        exit(1);
    }

    size_t bytesWritten = fwrite(tpl->data, tupleSize, 1, f);
    assert(bytesWritten > 0);
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
    execute(queryplan, handleTupleInsert);

    /* Clean up */
    fclose(f);
    freeQueryplan(queryplan);
}

