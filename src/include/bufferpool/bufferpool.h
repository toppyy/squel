#pragma once
#include <stdlib.h>
#include "../const.h"
#include "../parser/parsetree.h"

/*


*/


typedef struct Tuple {
    size_t  columnCount;
    size_t  identifiers[ARRAYMAXSIZE];
    size_t  size;
    char*   pCols[ARRAYMAXSIZE];
    enum Datatype datatypes[ARRAYMAXSIZE];
} Tuple;

typedef struct {
    Tuple tuples[QUERYBUFFER];
    size_t tupleCount;
} TupleBuffer;


// void freeTuple(size_t idx, char* memory);