#pragma once
#include <stddef.h>
#include <stdlib.h>
#include <string.h>

#define MAXCOLS 100

typedef enum Tupletype {
    TPL_TDB,
    TPL_DELIMITED
} Tupletype;


typedef struct  {
    void*   data;
    size_t  size;
    size_t  longCount;
    char    tdbOffsets;
    char    casted[MAXCOLS];
    size_t  offsets[MAXCOLS];
    size_t  sizes[MAXCOLS];
    long    longs[MAXCOLS];
    Tupletype type;
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

void castColumnToLong(Tuple* tpl, size_t colOffset);