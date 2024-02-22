#pragma once
#include <stdbool.h>
#include <stdio.h>
#include "../io/io.h"
#include "../parser/utils.h"
#include "../parser/parsetree.h"

#define PROJCOLSIZE 50
#define FILTERSIZE  50
#define JOINSIZE 100
#define NAMESIZEMAX 255

typedef enum {
    OP_SCAN,
    OP_PROJECT,
    OP_FILTER,
    OP_JOIN,
    OP_AGGREGATE
} OperatorType;

typedef enum {
    COUNT,
    AVG,
    SUM
} AggregationType;


#define METADATASIZE    100
#define COLUMNSSIZE     10

typedef struct {
    char name[NAMESIZEMAX];
    Datatype type;
    size_t identifier;
} ColumnMetadata;



typedef struct {
    char path[METADATASIZE];
    char alias[METADATASIZE];
    ColumnMetadata columns[COLUMNSSIZE];
    size_t columnCount;
} TableMetadata;



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
    char                charConstants[FILTERSIZE][FILTERSIZE];
    int                 intConstants[FILTERSIZE];
    enum nodeType       exprTypes[FILTERSIZE];
    int                 boolExprList[FILTERSIZE];
    size_t              boolExprListSize;
    struct Operator*    next;
    enum nodeType       operatorNext;
} FilterInfo;

typedef struct {
    struct Operator* left;
    struct Operator* right;
    struct Operator* filter;
    struct Tuple* lastTuple;
    struct Tuple* rightTuples[JOINSIZE];
    size_t rightTupleIdx;
    size_t rightTupleCount;
    bool rightTuplesCollected;
} JoinInfo;

typedef struct {
    bool aggregationDone;
    AggregationType aggtype;
    int colToAggregate;
} AggInfo;


typedef union {
    ProjectInfo project;
    ScanInfo    scan;
    FilterInfo  filter;
    JoinInfo    join;
    AggInfo     aggregate;
} OperatorInfo;

typedef struct Operator {
    OperatorType type;
    OperatorInfo info;
    ResultSet resultDescription;
    struct Operator* child;
    struct Tuple* (*getTuple) (struct Operator* op);
} Operator;

void freeQueryplan(Operator *node);
void copyResultDescription(Operator* opFrom, Operator* opTo, size_t offset);
void catalogFile(const char* path, TableMetadata* p_tablemetadata, char delimiter);


Operator* makeProjectOp(struct Node* node, struct Operator* child_op);
Operator* makeScanOp(Node* node);
Operator* makeFilterOps(Node* where_node, Operator* child);
Operator* makeAggregateOp(Node* node, Operator* child_op);
Operator* makeJoinOp(Node* node);


Operator* planQuery(Node* astRoot) ;