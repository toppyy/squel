#pragma once
#include "tuple.h"

typedef struct {
    Tuple** tuples;
    size_t size;
    size_t capacity;
} TupleBuffer;

TupleBuffer* initTupleBuffer(size_t p_capacity);
void resizeTupleBuffer(TupleBuffer* buff);
void addTupleToBuffer(Tuple* tpl, TupleBuffer* buff);
void freeTupleBuffer(TupleBuffer* buff);
Tuple* getTupleByIndex(TupleBuffer* buff, size_t idx);
size_t isTupleBufferEmpty(TupleBuffer* buff);
