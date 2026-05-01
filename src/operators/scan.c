#include "../include/operators/scan.h"

void scanGetTuple(Operator* op, Tuple* tpl) {

    checkPtrNotNull(op, "NULL pointer passed to scanGetTuple");

    // Read header and first line of data
    FILE* f = op->info.scan.tablefile;
    if (f == NULL) {
        op->info.scan.buffer = malloc(LINEBUFF);
        f = fopen(op->info.scan.table.path, "r");
        // Read header and discard it (already used by catalogTable)
        readLineToBuffer(f, op->info.scan.buffer, LINEBUFF);
        op->info.scan.tablefile = f;
        // free(buff);
    }


    char* lineBuffer = op->info.scan.buffer;
    char* line = readLineToBuffer(f, lineBuffer, LINEBUFF);

     if (line == NULL) {
        free(lineBuffer);
        fclose(op->info.scan.tablefile);
        markTupleAsEmpty(tpl);
        return;
    }


    // Calculate offsets to each column
    size_t  cursor  = 0,
            i       = 0,
            strLen  = 0,
            tplSize = 0;

    char* dlmtr = lineBuffer;

    // size_t longSize = sizeof(long);
    // size_t intSize  = sizeof(int);

    void* diskBuffer = tpl->data;
    void* diskBufferCursor = diskBuffer;
    
    // TableMetadata* tbldef = &op->info.scan.table;

    for (;;) {

        dlmtr = strchr(lineBuffer + cursor, DELIMITER);

        if (dlmtr != NULL)  {
            (*dlmtr) = '\0'; // Replace delimiter with NULL so each column is a NULL-terminated string
        }

        // Create TDB representation:
        //  strings as is + strlen
        //  integers as atoi

        // switch (tbldef->columns[i].type) {
        //     case DTYPE_STR:
        strLen = strlen(lineBuffer + cursor) + 1;
        if (strLen > TDBMAXSTRINGSIZE) {
            printf("String data is truncated due to column size\n");
            strLen = TDBMAXSTRINGSIZE;
        }
        memcpy(diskBufferCursor, lineBuffer + cursor, TDBMAXSTRINGSIZE);
        diskBufferCursor += strLen;
        memset(diskBufferCursor, 0, TDBMAXSTRINGSIZE - strLen);
        diskBufferCursor += TDBMAXSTRINGSIZE - strLen;
        op->resultDescription.pCols[i] = tplSize;
        tplSize += TDBMAXSTRINGSIZE;
        tpl->casted = 0;

        //         break;

        //     case DTYPE_INT:
        //         int tmp = atoi(lineBuffer + cursor);
        //         memcpy(diskBufferCursor, &tmp, intSize);
        //         diskBufferCursor += intSize;
        //         op->resultDescription.pCols[i] = tplSize;
        //         tplSize += intSize;

        //         break;

        //     case DTYPE_LONG:
        //         // long ltmp = atol(lineBuffer + cursor);
        //         memcpy(diskBufferCursor, &ltmp, longSize);
        //         diskBufferCursor += longSize;
        //         op->resultDescription.pCols[i] = tplSize;
        //         tplSize += longSize;
        //         break;

        //     default:
        //         printf("Don't know how to represent type %d in TDB-format\n", tbldef->columns[i].type);
        //         exit(1);

        // }

        if (dlmtr == NULL) {
            break;
        }

        cursor += dlmtr - (lineBuffer + cursor) + 1;

        i++;
    };

    // // ---------------- Useful for debuggin. Leave it be for a while ------------------
    // tpldata = diskBuffer;
    // printf("tpldata at: ", diskBuffer);
    // for (size_t i = 0; i < op->resultDescription.size; i++) {
    //     printf("%d, ", *(char*) (tpldata + i));
    // }
    // printf("\n");

    // printf("tpldata-cols:\n");
    // char testi[CHARMAXSIZE];
    // char* ptr = testi;
    // for (size_t i = 0; i < op->resultDescription.columnCount; i++) {
    //     memset(testi,0,CHARMAXSIZE);
    //     getColAsChar(ptr, offset, op->resultDescription.pCols[i], tbldef.columns[i].type);
    //     printf("\t%s (%ld)\n",testi, op->resultDescription.pCols[i]);
    // }
    // printf("\n");

    // op->info.scan.cursor += len;
    op->resultDescription.size = tplSize;

    // free(lineBuffer);
}

