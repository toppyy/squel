#include "../include/odbc/odbc.h"


SQLRETURN  SQL_API SQLExecute(SQLHSTMT stmtHandle __attribute__((unused))) {


    // StatementHandle* handle = (StatementHandle*) stmtHandle;


    printf("SQLExecute called\n");
    return SQL_SUCCESS;
}
