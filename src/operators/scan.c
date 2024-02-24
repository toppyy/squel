#include "../include/operators/scan.h"

int scanGetTuple(Operator* op) {

    checkPtrNotNull(op, "NULL pointer passed to scanGetTuple");

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


    Tuple* tpl = addTuple();
    char* line = readLineToBuffer(f, tpl->data, TUPLEDATAMAXSIZE);

     if (line == NULL) {
        // TODO: free tuple
        fclose(op->info.scan.tablefile);
        return -1;
    }
    size_t len = strlen(line);

    tpl->columnCount = op->resultDescription.columnCount;
    tpl->size        = len;

    // Calculate offsets to each column
    char* dlmtr = NULL;

    size_t cursor = 0;
    size_t i = 1;

    char* ptrData = tpl->data;
    tpl->pCols[0] = 0;

    for (;;) {
        
        dlmtr = strchr(ptrData + cursor, DELIMITER);
        
        if (dlmtr == NULL) {
            tpl->pCols[i] =  cursor;
            break;
        }
        cursor += dlmtr - (ptrData + cursor) + 1;
        
        tpl->pCols[i] = cursor;
        
        (*dlmtr) = '\0'; // Replace delimiter with NULL so each column is a NULL-terminated string
        i++;
    
    };
    
    op->info.scan.cursor += len;

    return tpl->idx;
}

