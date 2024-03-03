#include "../include/bufferpool/bufferpool.h"
#include "../include/planner/planner.h"


void growBufferpoolIfNeedBe(size_t size) {
    
    if (buffpool->used + (long) size < buffpool->capacity) return;
    long oldCapacity = buffpool->capacity;
    buffpool->capacity *= 2;
    buffpool->pool = realloc(buffpool->pool, buffpool->capacity);
    checkPtrNotNull(buffpool->pool, "Could not allocate memory for bufferpool");
    memset(buffpool->pool + oldCapacity, 0, oldCapacity);
    
}

void copyToBufferPool(int destinationoffset, void* source, size_t size) {
    growBufferpoolIfNeedBe(size);
    void* destination = getTuple(destinationoffset);
    memcpy(destination, source, size);
    buffpool->used += size;
}

int addToBufferPoolFromOffset(int originOffset, size_t size) {
    growBufferpoolIfNeedBe(size);
    void* target = getNextFreeSlot();
    memcpy(target, getTuple(originOffset), size);
    int offset = buffpool->used;
    buffpool->used += size;
    return offset;
}


int addToBufferPool(void* source, size_t size) {
    growBufferpoolIfNeedBe(size);
    void* target = getNextFreeSlot();
    memcpy(target, source, size);
    int offset = buffpool->used;
    buffpool->used += size;
    return offset;
}

void reserveSpaceBufferpool(int offset, size_t size) {
    growBufferpoolIfNeedBe(size);
    void* from = getTuple(offset);
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


