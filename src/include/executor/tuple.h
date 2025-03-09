#pragma once
#include <stddef.h>
#include <stdlib.h>

typedef struct  {
    void* data;
    size_t size;
} Tuple;




Tuple* initTuple();

Tuple* initTupleOfSize(size_t p_size);

void* getTupleCol(Tuple* tpl, size_t colOffset);

void freeTuple(Tuple* tpl);

size_t isTupleEmpty(Tuple* tpl);

void markTupleAsEmpty(Tuple* tpl);
