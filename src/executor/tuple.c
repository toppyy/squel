#include "../include/executor/tuple.h"


Tuple* initTuple() {
    Tuple* tpl = malloc(sizeof(Tuple)); // Heap allocation
    tpl->size = 0;
    return tpl;
}


Tuple* initTupleOfSize(size_t p_size) {
    Tuple* tpl = malloc(sizeof(Tuple)); // Heap allocation
    tpl->data = calloc(1, p_size);
    tpl->size = p_size;
    return tpl;
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