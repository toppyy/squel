#pragma once
#include <stdbool.h>
#include <stdio.h>
#include "../const.h"
#include "../io/flatfile.h"
#include "../io/tdb.h"
#include "../parser/utils.h"
#include "../parser/parsetree.h"
#include "../executor/tuple.h"
#include "../executor/tuplebuffer.h"
#include "../util/hashmap.h"
#include "../util/options.h"


typedef enum {
    OP_SCANTDB,
    OP_SCAN,
    OP_PROJECT,
    OP_FILTER,
    OP_JOIN,
    OP_AGGREGATE,
    OP_HASHJOIN,
    OP_STMTCREATE,
    OP_STMTINSERT,
    OP_STMTEXPLAIN
} OperatorType;

typedef enum ComparisonType {
    CMP_COL_COL,
    CMP_CONST_CONST,
    CMP_CONST_COL,
    CMP_COL_CONST
} ComparisonType;

typedef enum {
    COUNT,
    AVG,
    SUM,
    MAX,
    MIN
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
    size_t pCols[ARRAYMAXSIZE];
    size_t size;
} ResultSet;

typedef struct {
    char    columnsToProject[ARRAYMAXSIZE][CHARMAXSIZE];
    int     colRefs[ARRAYMAXSIZE];
    int     colCount;
} ProjectInfo;

typedef struct {
    TableMetadata table;
    FILE* tablefile;
    int fd;
    void*  buffer;
    size_t recordsInBuffer;
    size_t bufferSize;
    bool   fileRead;
    size_t cursor;
    struct TDB tbldef;
    size_t recordSize;
    size_t columnOffsets[ARRAYMAXSIZE];
} ScanInfo;



typedef struct FilterInfo {
    char                charConstants[ARRAYMAXSIZE][CHARMAXSIZE];
    long                numConstants[ARRAYMAXSIZE];
    enum Datatype       exprDatatypes[ARRAYMAXSIZE];
    enum nodeType       exprTypes[ARRAYMAXSIZE];
    int                 boolExprList[ARRAYMAXSIZE];
    size_t              boolExprListSize;
    struct Operator*    next;
    enum nodeType       operatorNext;
    ComparisonType      compType;
    int (*filter) (void* expr1, void* expr2);
} FilterInfo;

typedef struct {
    struct Operator* left;
    struct Operator* right;
    struct Operator* filter;
    Hashmap* hashmap;
    TupleBuffer* rightTuples;
    Tuple* leftTuple;
    size_t rightTupleIdx;
    size_t rightTupleCount;
    bool rightTuplesCollected;
} JoinInfo;

typedef struct {
    bool aggregationDone;
    AggregationType aggtype;
    size_t colToAggregate;
} AggInfo;


typedef struct {
    Node* columnList;
    char objectName[CHARMAXSIZE];
} CreateInfo;

typedef struct {
    char targetTableName[CHARMAXSIZE];
} InsertInfo;

typedef union {
    ProjectInfo     project;
    ScanInfo        scan;
    FilterInfo      filter;
    JoinInfo        join;
    AggInfo         aggregate;
    CreateInfo      create;
    InsertInfo      insert;
} OperatorInfo;

typedef struct Operator {
    OperatorType type;
    OperatorInfo info;
    ResultSet resultDescription;
    int iteratorTupleOffset;
    struct Operator* child;
    void (*getTuple) (struct Operator* op, Tuple* tpl);
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
Operator* planSelect(Node* astRoot) ;