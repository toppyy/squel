#include "../include/odbc/odbc.h"


SQLRETURN SQLFetchScroll(  
      SQLHSTMT      StatementHandle,  
      SQLSMALLINT   FetchOrientation,  
      SQLLEN        FetchOffset __attribute__((unused))
)
{
    switch(FetchOrientation) {
        case SQL_FETCH_NEXT:
            return _Fetch(StatementHandle);
        default:
            SQL_ERROR;
    }

    return SQL_ERROR;
}