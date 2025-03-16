#pragma once
#include "tuple.h"
#include <stdio.h>

typedef struct {
    Tuple* tuples;
    void*  data;
    size_t cursor;
    size_t tupledatasize;
    size_t size;
    size_t capacity;
} TupleBuffer;

TupleBuffer* initTupleBuffer(size_t p_capacity, size_t p_tuplesize);
void resizeTupleBuffer(TupleBuffer* buff);
void addTupleToBuffer(Tuple* tpl, TupleBuffer* buff);
void freeTupleBuffer(TupleBuffer* buff);
void updateTupleDataptr(TupleBuffer* buff, Tuple* tpl, size_t idx);
Tuple* getTupleByIndex(TupleBuffer* buff, size_t idx);
Tuple* getTupleFromBuffer(TupleBuffer* buff);
size_t isTupleBufferEmpty(TupleBuffer* buff);
