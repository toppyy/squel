#include "../include/bufferpool/bufferpool.h"


Tuple* addTuple() {
    tplbuffer->tupleCount++;
    size_t idx = tplbuffer->tupleCount;
    return &tplbuffer->tuples[idx];
}
