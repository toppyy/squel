#include "../include/odbc/odbc.h"



RETCODE SQL_API _InitQuery(SQLHSTMT stmtHandle, SQLCHAR* stmt) {

    StatementHandle* handle = (StatementHandle*) stmtHandle;

    if (handle->queryplan != NULL) {
        // Free any previously allocated stuff
        freeQueryplan(((StatementHandle*) handle)->queryplan);
        freeTuple(((StatementHandle*) handle)->tpl);
    }

    handle->queryplan = planQuery((char*) stmt);
    prepareQuery(handle->queryplan);

    handle->tpl = initTupleOfSize(TUPLESIZE);
    return SQL_SUCCESS;

}


