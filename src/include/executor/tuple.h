#pragma once
#include <stddef.h>
#include <stdlib.h>
#include <string.h>

#define MAXCOLS 100

typedef struct  {
    void* data;
    size_t size;
    char   casted[MAXCOLS];
    size_t offsets[MAXCOLS];
    size_t sizes[MAXCOLS];

    /* Data */
    size_t  longCount;
    long longs[MAXCOLS];

} Tuple;




Tuple* initTuple();

Tuple* initTupleOfSize(size_t p_size);

void* getTupleColByIndex(Tuple* tpl, size_t index);
long getTupleLongColByIndex(Tuple* tpl, size_t index);

void* getTupleCol(Tuple* tpl, size_t colOffset);

void resetTuple(Tuple* tpl);

void freeTuple(Tuple* tpl);

size_t isTupleEmpty(Tuple* tpl);

void markTupleAsEmpty(Tuple* tpl);


long castColumnToLong(Tuple* tpl, size_t colOffset);