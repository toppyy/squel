#include "../include/odbc/odbc.h"

SQLRETURN SQLFreeStmt(  
     SQLHSTMT       StatementHandle __attribute__((unused)),
     SQLUSMALLINT   Option __attribute__((unused))
)
{
    return SQL_SUCCESS;
}

