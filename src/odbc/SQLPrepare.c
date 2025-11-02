#include "../include/odbc/odbc.h"



SQLRETURN  SQL_API SQLPrepare(
    SQLHSTMT stmtHandle __attribute__((unused)),
    SQLCHAR* stmt __attribute__((unused)),
    SQLINTEGER TextLength __attribute__((unused))
) {
    
    StatementHandle* handle = (StatementHandle*) stmtHandle;

    handle->queryplan = planQuery((char*) stmt);
    prepareQuery(handle->queryplan);

    handle->tpl = initTupleOfSize(TUPLESIZE);

    return SQL_SUCCESS;
}