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




    char* lineBuffer = malloc(LINEBUFF);
    char* line = readLineToBuffer(f, lineBuffer, LINEBUFF);

     if (line == NULL) {
        free(lineBuffer);
        fclose(op->info.scan.tablefile);
        return -1;
    }


    size_t len = strlen(line);

    // Calculate offsets to each column


    size_t cursor = 0;
    size_t i = 0, strLen = 0;

    char* ptrData = lineBuffer;
    char* dlmtr = ptrData;

    // Table metadata
    TableMetadata tbldef = op->info.scan.table;

    // Location in the bufferpool to store tuple data
    void* diskBufferCursor = getNextFreeSlot();
    int offset = getCurrentOffset();

    size_t tplSize = 0;


    // Find pointers to each column
    for (;;) {



        dlmtr = strchr(ptrData + cursor, DELIMITER);


        if (dlmtr != NULL)  {
            (*dlmtr) = '\0'; // Replace delimiter with NULL so each column is a NULL-terminated string
        }

        // Create TDB representation:
        //  strings as is + strlen
        //  integers as atoi

        size_t longSize = sizeof(long);
        size_t intSize  = sizeof(int);



        switch (tbldef.columns[i].type) {
            case DTYPE_STR:
                strLen = strlen(ptrData + cursor) + 1;
                if (strLen > TDBMAXSTRINGSIZE) {
                    printf("String data is truncated due to column size\n");
                    strLen = TDBMAXSTRINGSIZE;
                }
                copyToBufferPool(diskBufferCursor, ptrData + cursor, TDBMAXSTRINGSIZE);
                diskBufferCursor += strLen;
                // Pad the string with '\0'
                memset(diskBufferCursor, 0, TDBMAXSTRINGSIZE - strLen);
                diskBufferCursor += TDBMAXSTRINGSIZE - strLen;
                op->resultDescription.pCols[i] = tplSize;
                tplSize += TDBMAXSTRINGSIZE;

                break;

            case DTYPE_INT:
                int tmp = atoi(ptrData + cursor);
                copyToBufferPool(diskBufferCursor, &tmp, intSize);
                diskBufferCursor += intSize;
                op->resultDescription.pCols[i] = tplSize;
                tplSize += intSize;

                break;

            case DTYPE_LONG:
                long ltmp = atol(ptrData + cursor);
                copyToBufferPool(diskBufferCursor, &ltmp, longSize);
                diskBufferCursor += longSize;
                op->resultDescription.pCols[i] = tplSize;
                tplSize += longSize;
                break;

            default:
                printf("Don't know how to represent type %d in TDB-format\n", tbldef.columns[i].type);
                exit(1);

        }

        if (dlmtr == NULL) {
            break;
        }

        cursor += dlmtr - (ptrData + cursor) + 1;

        i++;
    };

    op->resultDescription.size = tplSize;

    // void* tpldata = getTuple(offset);
    // printf("tpldata: ");
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

    op->info.scan.cursor += len;

    free(lineBuffer);

    return offset;
}

