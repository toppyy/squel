#include "../include/executor/tuplebuffer.h"


TupleBuffer* initTupleBuffer(size_t p_capacity) {
    TupleBuffer* buff = malloc(sizeof(TupleBuffer));
    buff->capacity  = p_capacity;
    buff->tuples    = malloc(p_capacity * sizeof(Tuple*));
    buff->size      = 0;
    return buff;
}


void resizeTupleBuffer(TupleBuffer* buff) {
    buff->capacity *= 2;
    buff->tuples = realloc(buff->tuples, buff->capacity * sizeof(Tuple));
}

void addTupleToBuffer(Tuple* tpl, TupleBuffer* buff) {
    if (buff->size == buff->capacity) {
        resizeTupleBuffer(buff);
    }

    buff->tuples[buff->size++] = tpl;
}

void freeTupleBuffer(TupleBuffer* buff) {
    for (size_t i = 0; i < buff->size; i++) freeTuple(buff->tuples[i]);
    free(buff->tuples);
    free(buff);
}


Tuple* getTupleByIndex(TupleBuffer* buff, size_t idx) {
    return buff->tuples[idx];
}