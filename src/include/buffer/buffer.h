#pragma once
#include <stdlib.h>
#include "../parser/parsetree.h"
#define TUPLECOLS 10
#define TUPLEBUFFERSIZE 1000




typedef struct Tuple {
    size_t  columnCount;
    size_t  identifiers[TUPLECOLS];
    size_t  size;
    char*   pCols[TUPLECOLS];
    enum Datatype datatypes[TUPLECOLS];
} Tuple;

typedef struct {

    Tuple tuples[TUPLEBUFFERSIZE];
    size_t tupleCount;
} TupleBuffer;


// void freeTuple(size_t idx, char* memory);