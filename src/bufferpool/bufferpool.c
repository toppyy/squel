#include "../include/bufferpool/bufferpool.h"
#include "../include/planner/planner.h"

Tuple* addTuple() {
    if (tplbuffer->tupleCount + 1 >= tplbuffer->bufferSize) {
        // printf("Increasing bufferpool size to %ld from %ld\n", tplbuffer->bufferSize * 2,tplbuffer->bufferSize);
        tplbuffer->tuples = realloc(tplbuffer->tuples, tplbuffer->bufferSize * 2 * sizeof(Tuple));
        memset(tplbuffer->tuples + tplbuffer->bufferSize, 0, tplbuffer->bufferSize * sizeof(Tuple));
        tplbuffer->bufferSize *= 2;
        
        checkPtrNotNull(tplbuffer->tuples, "Unable to reallocate memory for tuplebuffer");
    }
    tplbuffer->tupleCount++;
    size_t idx = tplbuffer->tupleCount;
    tplbuffer->tuples[idx].pCols[0] = 0;
    tplbuffer->tuples[idx].idx = idx;
    tplbuffer->tuples[idx].poolOffset = buffpool->used;
    // Not reliable. May change if the buffer pool needs to be reallocated
    tplbuffer->tuples[idx].data = getNextFreeSlot();
    return &tplbuffer->tuples[idx];
}


void copyToBufferPool(void* destination, void* source, size_t size) {
    memcpy(destination, source, size);
    buffpool->used += size;
}

void reserveSpaceBufferpool(void* from, size_t size) {
    memset(from, 0, size);
    buffpool->used += size;
}


void* getCol(Tuple* tpl, size_t colIdx) {
    return tpl->data + tpl->pCols[colIdx];
}

Tuple* getTuple(int idx) {
    if (idx < 0) {
        return NULL;
    }
    Tuple* tpl = &tplbuffer->tuples[idx];
    tpl->data = buffpool->pool + tpl->poolOffset;
    return tpl;
}

void* getNextFreeSlot() {
    return buffpool->pool + buffpool->used;
}


void getColAsChar(char* target, Tuple* tpl,size_t colIdx, Datatype type) {
    if (type == DTYPE_STR) {
        strcpy(target, getCol(tpl, colIdx));
        return;
    }
    if (type == DTYPE_INT) {
        char tmp[CHARMAXSIZE];
        sprintf(tmp, "%d", *(int*) getCol(tpl, colIdx));
        memcpy(target, tmp, strlen(tmp));
        return;
    }
    if (type == DTYPE_LONG) {
        char tmp[CHARMAXSIZE];
        sprintf(tmp, "%ld", *(long*) getCol(tpl, colIdx));
        memcpy(target, tmp, strlen(tmp));
        return;
    }
    printf("Don't know how to represent type %d as char\n", type);
    exit(1);
}
