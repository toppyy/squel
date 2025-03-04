#pragma once
#include <stddef.h>
#include <stdlib.h>

typedef struct  {
    void* data;
    size_t size;
} Tuple;


Tuple* initTuple(size_t size);

void* getTupleCol(Tuple* tpl, size_t colOffset);