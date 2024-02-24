#include "../include/bufferpool/bufferpool.h"


Tuple* addTuple() {
    tplbuffer->tupleCount++;
    size_t idx = tplbuffer->tupleCount;
    tplbuffer->tuples[idx].pCols[0] = tplbuffer->tuples[idx].data;
    return &tplbuffer->tuples[idx];
}
