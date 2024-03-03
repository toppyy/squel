#pragma once
#include <stdlib.h>
#include "../const.h"
#include "../parser/parsetree.h"

/*
    The bufferpool maintains a pool for tuples
    to which tuples can be added and removed (freed) from.

    A tuple is struct with a fixed size despite
    the fact that the data is likely not fixed in size.

    A tuple contains it's data as a string. Columns
    are pointers to the string.

    The rest of the system passes around pointers to the buffer pool.

*/


typedef struct {
    void* pool;
    long capacity;
    long used;
} Bufferpool;

extern Bufferpool* buffpool;


void* getNextFreeSlot();
void getColAsChar(char* target, int pooloffset, size_t colIdx, Datatype type);
void copyToBufferPool(int destinationoffset, void* source, size_t size);
int addToBufferPool(void* source, size_t size);
int addToBufferPoolFromOffset(int offset, size_t size);
void reserveSpaceBufferpool(int offset, size_t size);

int getCurrentOffset();

void* getTuple(int pooloffset);
void* getCol(int pooloffset, size_t colOffset);