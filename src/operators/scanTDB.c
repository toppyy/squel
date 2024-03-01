#include "../include/operators/scanTDB.h"

int scanTDBGetTuple(Operator* op) {

    checkPtrNotNull(op, "NULL pointer passed to scanTDBGetTuple");


    int fd = op->info.scan.fd;
    if (fd == 0) {
        fd = open(op->info.scan.tbldef.path, O_RDONLY);
        lseek(fd, op->info.scan.cursor, SEEK_SET);
    }
    
    ssize_t bytesRead = read(fd , op->info.scan.buffer, op->info.scan.recordSize);

    if (bytesRead < 0) {
        printf("Error reading the file at '%s'\n", op->info.scan.tbldef.path);
        exit(1);
    }


    if (bytesRead == 0) {
        free(op->info.scan.buffer);
        close(fd);
        return -1;
    }


    op->info.scan.cursor += op->info.scan.recordSize;

    Tuple* tpl = addTuple();

    memcpy(tpl->pCols, op->info.scan.columnOffsets, ARRAYMAXSIZE);

    copyToBufferPool(tpl->data, op->info.scan.buffer, op->info.scan.recordSize);


    return tpl->idx;
}