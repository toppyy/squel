#include "../include/operators/scanTDB.h"


void fillBuffer(Operator* op) {

    int fd = op->info.scan.fd;

    if (fd == 0) {
        fd = open(op->info.scan.tbldef.path, O_RDONLY);
        op->info.scan.fd = fd;
        lseek(fd, op->info.scan.cursor, SEEK_SET);        
    }

    ssize_t bytesRead = read(fd, op->info.scan.buffer, op->info.scan.bufferSize);

    if (bytesRead < 0) {
        printf("ScanTDB - Error reading the file at '%s': %s\n", op->info.scan.tbldef.path, strerror(errno));
        exit(1);
    }

    op->info.scan.cursor += bytesRead;
    op->info.scan.recordsInBuffer = ( bytesRead / op->info.scan.recordSize );


    if (bytesRead < (ssize_t) op->info.scan.bufferSize) {
        op->info.scan.fileRead = true;
        close(fd);
    }

}

Tuple* scanTDBGetTuple(Operator* op) {

    checkPtrNotNull(op, "NULL pointer passed to scanTDBGetTuple");

    if (op->info.scan.fileRead && op->info.scan.recordsInBuffer == 0) {
        free(op->info.scan.buffer);
        return NULL;
    }

    if (op->info.scan.recordsInBuffer == 0) {
        fillBuffer(op);
        return scanTDBGetTuple(op);
    }

    size_t bufferDataOffset = (op->info.scan.recordsInBuffer - 1) * op->info.scan.recordSize;    
    op->info.scan.recordsInBuffer--;

    // Write to bufferpool
    if (op->iteratorTupleOffset == -1) {
        op->iteratorTupleOffset = addToBufferPool(op->info.scan.buffer + bufferDataOffset, op->info.scan.recordSize);
    } else {
        copyToBufferPool(op->iteratorTupleOffset, op->info.scan.buffer + bufferDataOffset, op->info.scan.recordSize);
    }


    Tuple* tpl = initTuple();
    tpl->data = op->info.scan.buffer + bufferDataOffset;

    return tpl;
}