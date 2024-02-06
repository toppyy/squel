#pragma once

#include "../parser/parsetree.h"
#include "../catalog/catalog.h"

#define PROJCOLSIZE 10
#define FILTERSIZE  20

typedef enum {
    OP_SCAN,
    OP_PROJECT,
    OP_FILTER,
} OperatorType;

typedef struct {
    size_t columnCount;
    ColumnMetadata columns[PROJCOLSIZE];

} ResultSet;

typedef struct {
    char    columnsToProject[PROJCOLSIZE][PROJCOLSIZE];
    int     colRefs[PROJCOLSIZE];
    int     tblRefs[PROJCOLSIZE];
    int     colCount;
} ProjectInfo;

typedef struct {
    TableMetadata table;
    FILE* tablefile;
    size_t cursor;
} ScanInfo;

typedef struct {
    int     boolExprList[FILTERSIZE];
    size_t  boolExprListSize;
} FilterInfo;


typedef union {
    ProjectInfo project;
    ScanInfo    scan;
    FilterInfo  filter;
} OperatorInfo;

typedef struct Operator {
    OperatorType type;
    OperatorInfo info;
    ResultSet resultDescription;
    struct Operator* next;
    struct Operator* child;
    struct Tuple* (*getTuple) (struct Operator* op);
} Operator;

void freeQueryplan(Operator *node);

Operator* planQuery(Node* astRoot, TableMetadata* tables, size_t tableCount) ;