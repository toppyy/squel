#pragma once
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sql.h>
#include <sqlext.h>
#include "../planner/planner.h"
#include "../api/api.h"


#define MAXCOLSOFTYPE 100



typedef struct {
    SQLSMALLINT type;
} Handle;

typedef struct {
    SQLSMALLINT calls;
    SQLSMALLINT type;
    Operator*   queryplan;
    Tuple*      tpl;
    long        rowcount;
    bool        bound;

    unsigned char   *charBuff[MAXCOLSOFTYPE];
    long            charBuffLength[MAXCOLSOFTYPE];
    long            *longintBuff[MAXCOLSOFTYPE];
} StatementHandle;

RETCODE SQL_API _Fetch(SQLHSTMT stmthandle);
SQLRETURN SQL_API _InitQuery(SQLHSTMT stmtHandle, SQLCHAR* stmt);
