#include "../include/odbc/odbc.h"


RETCODE SQL_API SQLFetch(SQLHSTMT stmthandle) {

    StatementHandle* handle = (StatementHandle*) stmthandle;

    fetchTuple(handle->queryplan, handle->tpl);

    if (isTupleEmpty(handle->tpl)) return SQL_NO_DATA;

    handle->rowcount++;

    // if (handle->calls++ > 5) return SQL_NO_DATA;


    return SQL_SUCCESS;
}

