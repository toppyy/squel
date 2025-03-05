#include "../include/executor/tuple.h"


Tuple* initTuple() {
    Tuple* tpl = malloc(sizeof(Tuple)); // Heap allocation
    tpl->size = 0;
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