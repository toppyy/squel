#include "../include/executor/tuple.h"


Tuple* initTuple(size_t size) {
    Tuple* tpl = malloc(sizeof(Tuple)); // Heap allocation
    tpl->size = size;
    return tpl;
}