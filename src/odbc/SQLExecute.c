#include "../include/odbc/odbc.h"


SQLRETURN  SQL_API SQLExecute(SQLHSTMT stmtHandle __attribute__((unused))) {

    // ODBC driver manager requires this. We just don't do anything.
    // The query is executed when getTuple is called on the queryplan.
    return SQL_SUCCESS;
}
