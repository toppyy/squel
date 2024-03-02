#include "../include/bufferpool/bufferpool.h"
#include "../include/planner/planner.h"


void copyToBufferPool(void* destination, void* source, size_t size) {
    memcpy(destination, source, size);
    buffpool->used += size;
}

int addToBufferPool(void* source, size_t size) {
    void* target = getNextFreeSlot();
    memcpy(target, source, size);
    int offset = buffpool->used;
    buffpool->used += size;
    return offset;
}

void reserveSpaceBufferpool(void* from, size_t size) {
    memset(from, 0, size);
    buffpool->used += size;
}


int getCurrentOffset() { return buffpool->used; }

void* getCol(int pooloffset, size_t colOffset) {
    return buffpool->pool + pooloffset + colOffset;
}

void* getTuple(int pooloffset) {
    return buffpool->pool + pooloffset;
}

void* getNextFreeSlot() {
    return buffpool->pool + buffpool->used;
}

void getColAsChar(char* target, int pooloffset, size_t colOffset, Datatype type) {
    if (type == DTYPE_STR) {
        strcpy(target, getCol(pooloffset, colOffset));
        return;
    }
    if (type == DTYPE_INT) {
        char tmp[CHARMAXSIZE];
        sprintf(tmp, "%d", *(int*) getCol(pooloffset, colOffset));
        memcpy(target, tmp, strlen(tmp));
        return;
    }
    if (type == DTYPE_LONG) {
        char tmp[CHARMAXSIZE];
        sprintf(tmp, "%ld", *(long*) getCol(pooloffset, colOffset));
        memcpy(target, tmp, strlen(tmp));
        return;
    }
    printf("Don't know how to represent type %d as char\n", type);
    exit(1);
}


