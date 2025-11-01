#pragma once
#include "../planner/planner.h"

typedef enum ctype {
    CTYPE_CHAR,
    CTYPE_INT,
    CTYPE_LONG
} ctype;

size_t getResultSetColumnCount(Operator* queryplan);

const char* getResultSetColumnName(Operator* queryplan, size_t colIdx);

ctype getResultSetColumnType(Operator* queryplan, size_t colIdx);

int isStatement(Operator* op);

void prepareQuery(Operator* queryplan);

void fetchTuple(Operator* op, Tuple* tpl);