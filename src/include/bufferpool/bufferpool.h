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


typedef struct Tuple {
    size_t  columnCount;
    size_t  size;
    size_t  idx;
    char    data[TUPLEDATAMAXSIZE]; 
    size_t  pCols[ARRAYMAXSIZE];
} Tuple;

typedef struct {
    Tuple* tuples;
    size_t tupleCount;
    size_t bufferSize;
} TupleBuffer;

extern TupleBuffer* tplbuffer;


Tuple* addTuple();
char* getCol(Tuple* tpl, size_t colIdx);
Tuple* getTuple(int idx);
