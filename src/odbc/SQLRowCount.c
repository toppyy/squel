#include "../include/odbc/odbc.h"


SQLRETURN SQLRowCount(
    SQLHSTMT stmtHandle __attribute__((unused)),
    SQLLEN  *pnRowCount
)  {
    // Should return the number of affected rows
    (*pnRowCount) = 0; 
    return SQL_SUCCESS;
}