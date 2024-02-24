#include "../include/operators/scan.h"

Tuple* scanGetTuple(Operator* op) {

    if (op == NULL) {
        printf("NULL pointer passed to scanGetTuple\n");
        exit(1);
    }

    // Read header and first line of data
    FILE* f = op->info.scan.tablefile;
    if (f == NULL) {
        char* buff = malloc(LINEBUFF);
        f = fopen(op->info.scan.table.path, "r");
        // Read header and discard it (already used by catalogTable)
        readLineToBuffer(f, buff, LINEBUFF); 
        op->info.scan.tablefile = f;
        free(buff);    
    }

    char* buffercacheWithCursor = buffercache;
    char* line = readLineToBuffer(f, buffercacheWithCursor, QUERYBUFFER - buffercacheSize);

    if (line == NULL) {
        
        fclose(op->info.scan.tablefile);
        return NULL;

    }

    size_t len = strlen(line);

    Tuple* tpl = addTuple();
    tpl->columnCount = op->resultDescription.columnCount;
    tpl->size        = len;

    // Fill datatypes for tuple
    // TODO: useless work here? use child resultDescription
    for (size_t i = 0; i < op->resultDescription.columnCount; i++) {
        tpl->datatypes[i] = op->resultDescription.columns[i].type;
    }
    // Add pointers to start of each column
    char* dlmtr = NULL;

    size_t cursor = 0;

    tpl->pCols[0] = buffercacheWithCursor;
    size_t i = 1;

    for (;;) {
        
        dlmtr = strchr(buffercacheWithCursor + cursor, DELIMITER);
        
        if (dlmtr == NULL) {
            tpl->pCols[i] =  buffercacheWithCursor;
            break;
        }
        cursor += dlmtr - (buffercacheWithCursor + cursor) + 1;
        
        tpl->pCols[i] =  buffercacheWithCursor + cursor;
        
        (*dlmtr) = '\0'; // Replace delimiter with NULL so each column is a NULL-terminated string
        i++;
    
    };

    
    op->info.scan.cursor += len;
    buffercache += len + 1;
    buffercacheSize += len + 1;

    return tpl;
}

