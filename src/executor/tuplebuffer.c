#include "../include/executor/tuplebuffer.h"



TupleBuffer* initTupleBuffer(size_t p_capacity, size_t p_tuplesize) {
    TupleBuffer* buff = malloc(sizeof(TupleBuffer));
    buff->capacity      = p_capacity;
    buff->tupledatasize = p_tuplesize;
    buff->tuples        = malloc(p_capacity * sizeof(Tuple));
    buff->data          = malloc(p_capacity * p_tuplesize);
    buff->size          = 0;
    buff->cursor        = 0;
    return buff;    
}


void resizeTupleBuffer(TupleBuffer* buff) {

    buff->capacity *= 2;

    Tuple* tmpTpl   = realloc(buff->tuples, buff->capacity * sizeof(Tuple));
    if (tmpTpl == NULL) {
        printf("ERROR: Could resize tuplebuffer from %ld to %ld\n", buff->capacity, buff->capacity * 2);
        exit(1);
    }

    buff->tuples    = tmpTpl;
    void* tmpData = realloc(buff->data, buff->capacity * buff->tupledatasize);
    if (tmpData == NULL) {
        printf("ERROR: Could resize tuplebuffer data from %ld to %ld\n", buff->capacity * buff->tupledatasize, buff->capacity * buff->tupledatasize * 2);
        exit(1);
    }

    buff->data = tmpData;
}


Tuple* getTupleFromBuffer(TupleBuffer* buff) {

    if (buff->size >= (buff->capacity-1)) {
        resizeTupleBuffer(buff);
    }

    Tuple* tpl = &buff->tuples[buff->size++];
    tpl->size = buff->tupledatasize;
    tpl->data = buff->data + buff->cursor;
    buff->cursor += buff->tupledatasize;
    return tpl;
}

void updateTupleDataptr(TupleBuffer* buff, Tuple* tpl, size_t idx) {
    tpl->data = buff->data + (idx * buff->tupledatasize);
}


void freeTupleBuffer(TupleBuffer* buff) {
    free(buff->tuples);
    free(buff->data);
    free(buff);
}


Tuple* getTupleByIndex(TupleBuffer* buff, size_t idx) {
    updateTupleDataptr(buff, &buff->tuples[idx], idx);
    return &buff->tuples[idx];
}

size_t isTupleBufferEmpty(TupleBuffer* buff) {
    if (buff->size > 0) {
        return 0;
    }
    return 1;
}