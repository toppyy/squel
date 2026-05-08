#include "../include/operators/scan.h"

void scanGetTuple(Operator* op, Tuple* tpl) {

    checkPtrNotNull(op, "NULL pointer passed to scanGetTuple");


    tpl->type = TPL_DELIMITED;
    
    // Read header and first line of data
    FILE* f = op->info.scan.tablefile;
    if (f == NULL) {
        op->info.scan.buffer = malloc(LINEBUFF);
        f = fopen(op->info.scan.table.path, "r");
        // Read header and discard it (already used by catalogTable)
        readLineToBuffer(f, op->info.scan.buffer, LINEBUFF);
        op->info.scan.tablefile = f;
    }
    
    // As we reuse tuples throughout the volcano, it needs to be reset
    resetTuple(tpl);
    
    char* line = readLineToBuffer(f, tpl->data, LINEBUFF);
    
    if (line == NULL) {
        free(op->info.scan.buffer);
        fclose(op->info.scan.tablefile);
        markTupleAsEmpty(tpl);
        return;
    }


    // Calculate offsets to each column
    size_t  cursor  = 0,
            i       = 0,
            strLen  = 0,
            tplSize = 0;

    char* dlmtr = tpl->data;

    for (;;) {

        dlmtr = strchr(tpl->data + cursor, DELIMITER);

        if (dlmtr != NULL)  {
            (*dlmtr) = '\0'; // Replace delimiter with NULL so each column is a NULL-terminated string
        }      
        
        strLen = dlmtr == NULL ? strlen(tpl->data + cursor) + 1 : (size_t)  ((void*) dlmtr - (tpl->data + cursor)) + 1;

        tpl->sizes[i] = strLen;
        tpl->offsets[i] = tplSize;
        tpl->casted[i] = 0;

        tplSize += strLen;

        if (dlmtr == NULL) {
            break;
        }

        cursor += strLen;
        i++;
    };


    op->resultDescription.size = tplSize;
    tpl->size = tplSize;

}

