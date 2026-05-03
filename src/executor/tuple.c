#include "../include/executor/tuple.h"

#include <stdio.h>

Tuple* initTuple() {
    Tuple* tpl = calloc(1, sizeof(Tuple));
    tpl->size = 0;
    return tpl;
}


Tuple* initTupleOfSize(size_t p_size) {
    Tuple* tpl = calloc(1, sizeof(Tuple));
    tpl->data = calloc(1, p_size);
    tpl->size = p_size;
    tpl->longCount = 0;
    return tpl;
}

void* getTupleColByIndex(Tuple* tpl, size_t index) {
    return tpl->data + tpl->offsets[index];
}


long getTupleLongColByIndex(Tuple* tpl, size_t index) {

    if (!tpl->casted[index]) {
        return castColumnToLong(tpl, index);
    };

    // printf("Accessing a tuple with argument index %ld. Index into longs is: %ld, char there is '%s'\n", index, tpl->offsets[index], (char*) getTupleColByIndex(tpl, index));
    
    return tpl->longs[tpl->offsets[index]];
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

void resetTuple(Tuple* tpl) {
    tpl->longCount = 0;
    memset(tpl->casted, 0, MAXCOLS);
}


long castColumnToLong(Tuple* tpl, size_t colIdx) {



    void* ptrToColData = getTupleColByIndex(tpl, colIdx);
    
    long colNumber = atol((char*) ptrToColData);

    tpl->offsets[colIdx] = tpl->longCount;
    tpl->longs[tpl->longCount++] = colNumber;    
    tpl->casted[colIdx] = 1;

    // if (strcmp((char*) tpl->data, "THAN") == 0) {
    // printf("castColumnToLong. colIdx: %ld, longCount: %ld\n", colIdx, tpl->longCount - 1);


    return colNumber;
}