#pragma once
#include <stdbool.h>
#include <stdio.h>
#include "../const.h"
#include "../io/flatfile.h"
#include "../io/tdb.h"
#include "../parser/utils.h"
#include "../parser/parsetree.h"


typedef enum {
    OP_SCANTDB,
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


typedef struct {
    char name[CHARMAXSIZE];
    char alias[CHARMAXSIZE];
    char resultSetAlias[CHARMAXSIZE];
    Datatype type;
    size_t identifier;
} ColumnMetadata;



typedef struct {
    char path[CHARMAXSIZE];
    char alias[CHARMAXSIZE];
    ColumnMetadata columns[ARRAYMAXSIZE];
    size_t columnCount;
} TableMetadata;



typedef struct {
    size_t columnCount;
    ColumnMetadata columns[ARRAYMAXSIZE];
} ResultSet;

typedef struct {
    char    columnsToProject[ARRAYMAXSIZE][CHARMAXSIZE];
    int     colRefs[ARRAYMAXSIZE];
    int     colCount;
} ProjectInfo;

typedef struct {
    TableMetadata table;
    FILE* tablefile;
    size_t cursor;
    struct TDB tbldef;
    size_t recordSize;
} ScanInfo;


typedef struct FilterInfo {
    char                charConstants[ARRAYMAXSIZE][CHARMAXSIZE];
    long                numConstants[ARRAYMAXSIZE];
    enum nodeType       exprTypes[ARRAYMAXSIZE];
    int                 boolExprList[ARRAYMAXSIZE];
    size_t              boolExprListSize;
    struct Operator*    next;
    enum nodeType       operatorNext;
} FilterInfo;

typedef struct {
    struct Operator* left;
    struct Operator* right;
    struct Operator* filter;
    struct Tuple* lastTuple;
    size_t rightTuples[JOINPTRBUFFER];
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
    int (*getTuple) (struct Operator* op);
} Operator;

void freeQueryplan(Operator *node);
void copyResultDescription(Operator* opFrom, Operator* opTo, size_t offset);
void checkPtrNotNull(void* node, char* msg);
void catalogFile(const char* path, TableMetadata* p_tablemetadata, char delimiter);
int findColIdxInResDesc(ResultSet* resultDesc, char* name, char* tblref);

Operator* makeProjectOp(struct Node* node, struct Operator* child_op);
Operator* makeScanOp(Node* node);
Operator* makeScanTDBOp(Node* node);
Operator* makeFilterOps(Node* where_node, Operator* child);
Operator* makeAggregateOp(Node* node, Operator* child_op);
Operator* makeJoinOp(Operator* left, Operator* right, Node* ON);

Operator* planQuery(Node* astRoot) ;