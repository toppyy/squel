#include "../include/odbc/odbc.h"


SQLRETURN SQLRowCount(
    SQLHSTMT stmtHandle __attribute__((unused)),
    SQLLEN  *pnRowCount
)  {
    (*pnRowCount) = ((StatementHandle*) stmtHandle)->rowcount;
    return SQL_SUCCESS;
}