#include "../include/odbc/odbc.h"


SQLRETURN SQLMoreResults(SQLHSTMT StmtHandle)
{
    StatementHandle* handle = (StatementHandle*) StmtHandle;
    if (isTupleEmpty(handle->tpl)) return SQL_NO_DATA;
    return SQL_SUCCESS;
}