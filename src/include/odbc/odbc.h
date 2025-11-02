#pragma once
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sql.h>
#include <sqlext.h>
#include "../planner/planner.h"
#include "../api/api.h"


typedef struct {
    SQLSMALLINT type;
} Handle;

typedef struct {
    SQLSMALLINT calls;
    SQLSMALLINT type;
    Operator*   queryplan;
    Tuple*      tpl;
    long        rowcount;
} StatementHandle;
