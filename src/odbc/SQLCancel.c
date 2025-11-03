#include "../include/odbc/odbc.h"


SQLRETURN SQLCancel(SQLHSTMT  StatementHandle __attribute__((unused)))
{
    return SQL_SUCCESS;
}