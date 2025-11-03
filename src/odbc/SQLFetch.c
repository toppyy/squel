#include "../include/odbc/odbc.h"


RETCODE SQL_API SQLFetch(SQLHSTMT stmthandle) {
    return _Fetch(stmthandle);
}

