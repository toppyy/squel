#pragma once

#include "../io/io.h"
#include "../parser/parsetree.h"
#include "../parser/utils.h"

/* Maintains a catalog for the data (basically access the filesystem and infers the metadata) */

#define METADATASIZE    100
#define COLUMNSSIZE     10

typedef enum {
    DTYPE_STR,
    DTYPE_INT,
} Datatype;

typedef struct {
    char name[METADATASIZE];
    Datatype type;
} ColumnMetadata;

typedef struct {
    char path[METADATASIZE];
    char alias[METADATASIZE];
    ColumnMetadata columns[COLUMNSSIZE];
    size_t columnCount;
} TableMetadata;



TableMetadata* catalogQuery(Node* astRoot, char delimiter);