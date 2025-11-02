#include "../include/odbc/odbc.h"



RETCODE SQL_API SQLNumResultCols(
        SQLHSTMT     stmtHandle,
        SQLSMALLINT *pccol) {
    
    (*pccol) = getResultSetColumnCount( ((StatementHandle*) stmtHandle)->queryplan );
    return SQL_SUCCESS;
}
