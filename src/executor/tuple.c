#include "../include/executor/tuple.h"


Tuple* initTuple() {
    Tuple* tpl = malloc(sizeof(Tuple));
    tpl->size = 0;
    return tpl;
}


Tuple* initTupleOfSize(size_t p_size) {
    Tuple* tpl = malloc(sizeof(Tuple));
    tpl->data = calloc(1, p_size);
    tpl->size = p_size;
    tpl->casted = 0;
    return tpl;
}


void* getTupleCol(Tuple* tpl, size_t colOffset) {
    return tpl->data + colOffset;
}

void freeTuple(Tuple* tpl) {
    if (tpl->data) {
        free(tpl->data);
    }
    free(tpl);
}

size_t isTupleEmpty(Tuple* tpl) {
    return tpl->size == 0 ? 1 : 0;
}

void markTupleAsEmpty(Tuple* tpl) {
    tpl->size = 0;
}

                    
long castColumnToLong(Tuple* tpl, size_t colOffset, size_t reservedSize) {

    size_t longSize = sizeof(long);

    void* ptrToColData = getTupleCol(tpl, colOffset);
    
    long colNumber = atol((char*) ptrToColData);

    memset(ptrToColData, 0, reservedSize);
    memcpy(ptrToColData, &colNumber, longSize);
    
    tpl->casted = 1;
    return colNumber;
}