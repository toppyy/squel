#include "../include/executor/tuple.h"


Tuple* initTuple(size_t size) {
    Tuple* tpl = malloc(sizeof(Tuple)); // Heap allocation
    tpl->size = size;
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