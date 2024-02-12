#include "../include/operators/scan.h"

Tuple* scanGetTuple(Operator* op) {

    if (op == NULL) {
        printf("NULL pointer passed to scanGetTuple\n");
        exit(1);
    }

    // Read header and first line of data
    FILE* f = op->info.scan.tablefile;
    if (f == NULL) {
        f = fopen(op->info.scan.table.path, "r");
        readLineToBuffer(f, bufferscan, SCANBUFFER); // Read header
        op->info.scan.tablefile = f;
    
    }

    char* buffercacheWithCursor = buffercache;
    char* line = readLineToBuffer(f, buffercacheWithCursor, QUERYBUFFER - buffercacheSize);

    if (line == NULL) {
        
        fclose(op->info.scan.tablefile);
        return NULL;

    }

    tplbuffer->tupleCount++;
    size_t idx = tplbuffer->tupleCount;    
    size_t len = strlen(line);

    tplbuffer->tuples[idx].columnCount = op->resultDescription.columnCount;
    tplbuffer->tuples[idx].size        = len;

    // Fill datatypes for tuple
    // TODO: useless work here? use child resultDescription
    for (size_t i = 0; i < op->resultDescription.columnCount; i++) {
        tplbuffer->tuples[idx].datatypes[i] = op->resultDescription.columns[i].type;
    }
    // Add pointers to start of each column
    char* dlmtr = NULL;

    size_t cursor = 0;

    tplbuffer->tuples[idx].pCols[0] = buffercacheWithCursor;
    size_t i = 1;

    for (;;) {
        
        dlmtr = strchr(buffercacheWithCursor + cursor, DELIMITER);
        
        if (dlmtr == NULL) {
            tplbuffer->tuples[idx].pCols[i] =  buffercacheWithCursor;
            break;
        }
        cursor += dlmtr - (buffercacheWithCursor + cursor) + 1;
        
        tplbuffer->tuples[idx].pCols[i] =  buffercacheWithCursor + cursor;
        
        (*dlmtr) = '\0'; // Replace delimiter with NULL so each column is a NULL-terminated string
        i++;
    
    };

    
    op->info.scan.cursor += len;
    buffercache += len + 1;
    buffercacheSize += len + 1;

    return &tplbuffer->tuples[idx];
}

