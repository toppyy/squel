#pragma once

#include "../parser/parsetree.h"
#include "../catalog/catalog.h"

#define PROJCOLSIZE 10
#define FILTERSIZE  20
#define JOINSIZE 100

typedef enum {
    OP_SCAN,
    OP_PROJECT,
    OP_FILTER,
    OP_JOIN
} OperatorType;

typedef struct {
    size_t columnCount;
    ColumnMetadata columns[PROJCOLSIZE];
} ResultSet;

typedef struct {
    char    columnsToProject[PROJCOLSIZE][PROJCOLSIZE];
    int     colRefs[PROJCOLSIZE];
    int     colCount;
} ProjectInfo;

typedef struct {
    TableMetadata table;
    FILE* tablefile;
    size_t cursor;
} ScanInfo;


typedef struct FilterInfo {
    char            charConstants[FILTERSIZE][FILTERSIZE];
    int             intConstants[FILTERSIZE];
    enum nodeType   exprTypes[FILTERSIZE];
    int             boolExprList[FILTERSIZE];
    size_t          boolExprListSize;
} FilterInfo;

typedef struct {
    struct Operator* left;
    struct Operator* right;
    struct FilterInfo filter;
    struct Tuple* last_tuple;
    struct Tuple* rightTuples[JOINSIZE];
    size_t rightTupleIdx;
    size_t rightTupleCount;
    bool rightTuplesCollected;
} JoinInfo;


typedef union {
    ProjectInfo project;
    ScanInfo    scan;
    FilterInfo  filter;
    JoinInfo    join;
} OperatorInfo;

typedef struct Operator {
    OperatorType type;
    OperatorInfo info;
    ResultSet resultDescription;
    struct Operator* child;
    struct Tuple* (*getTuple) (struct Operator* op);
} Operator;

void freeQueryplan(Operator *node);

Operator* planQuery(Node* astRoot) ;