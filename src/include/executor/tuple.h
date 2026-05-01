#pragma once
#include <stddef.h>
#include <stdlib.h>
#include <string.h>

typedef struct  {
    void* data;
    size_t size;
    char casted;
} Tuple;




Tuple* initTuple();

Tuple* initTupleOfSize(size_t p_size);

void* getTupleCol(Tuple* tpl, size_t colOffset);

void freeTuple(Tuple* tpl);

size_t isTupleEmpty(Tuple* tpl);

void markTupleAsEmpty(Tuple* tpl);


long castColumnToLong(Tuple* tpl, size_t colOffset, size_t reservedSize);