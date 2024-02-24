#include "../include/bufferpool/bufferpool.h"
#include "../include/planner/planner.h"

Tuple* addTuple() {
    if (tplbuffer->tupleCount + 1 >= tplbuffer->bufferSize) {
                
        tplbuffer->tuples = realloc(tplbuffer->tuples, tplbuffer->bufferSize * 2 * sizeof(Tuple));
        memset(tplbuffer->tuples + tplbuffer->bufferSize, 0, tplbuffer->bufferSize * sizeof(Tuple));
        tplbuffer->bufferSize *= 2;
        
        checkPtrNotNull(tplbuffer->tuples, "Unable to reallocate memory for tuplebuffer");
    }
    tplbuffer->tupleCount++;
    size_t idx = tplbuffer->tupleCount;
    tplbuffer->tuples[idx].pCols[0] = 0;
    return &tplbuffer->tuples[idx];
}


char* getCol(Tuple* tpl, size_t colIdx) {
    return tpl->data + tpl->pCols[colIdx];
}