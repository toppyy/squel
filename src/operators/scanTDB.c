#include "../include/operators/scanTDB.h"

int scanTDBGetTuple(Operator* op) {

    checkPtrNotNull(op, "NULL pointer passed to scanTDBGetTuple");

    // Read metadata to forward cursor by length of metadata
    FILE* f = op->info.scan.tablefile;
    if (f == NULL) {
        f = fopen(op->info.scan.tbldef.path, "r");
        readTdbMetadaFromFD(f);
    }

    void* data = malloc(op->info.scan.recordSize);

    if (data == NULL) {
        printf("Failed to allocate memory\n");
        exit(1);
    }

    int fd = open(op->info.scan.tbldef.path, O_RDONLY);
    lseek(fd, op->info.scan.cursor, SEEK_SET);
    size_t bytesRead = read(fd , data, op->info.scan.recordSize);
    close(fd);

    op->info.scan.cursor += op->info.scan.recordSize;


    if (bytesRead == 0) {
        free(data);
        return -1;
    }

    Tuple* tpl = addTuple();

    /*
    
        typedef struct Tuple {
            size_t  columnCount;
            size_t  size;
            size_t  idx;
            char    data[TUPLEDATAMAXSIZE]; 
            size_t  pCols[ARRAYMAXSIZE];
        } Tuple;

    */
    
    TDB tbldef = op->info.scan.tbldef;
    char tmp[CHARMAXSIZE];
    memset(&tmp, '\0', CHARMAXSIZE);
    void* ptr_data = data;
    for (size_t i = 0; i < tbldef.colCount; i++) {

        tpl->columnCount++;
        if (tbldef.datatypes[i] == DTYPE_STR) {
            tpl->pCols[i] = tpl->size;
            memcpy(getCol(tpl, i), ptr_data, tbldef.lengths[i]);
            tpl->size += tbldef.lengths[i];
            ptr_data += tbldef.lengths[i];
            
            continue;
        }

        if (tbldef.datatypes[i] == DTYPE_INT) {
            sprintf(tmp, "%d", *(int*) ptr_data);
            tpl->pCols[i] = tpl->size;
            memcpy(getCol(tpl, i), tmp, tbldef.lengths[i]);
            
            tpl->size += tbldef.lengths[i];
            ptr_data += tbldef.lengths[i];
            continue;
        }

        printf("Don't know how to turn type %d into tuple\n", tbldef.datatypes[i]);
        exit(1);

    }

    free(data);
    return tpl->idx;
}