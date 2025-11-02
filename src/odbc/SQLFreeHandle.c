

#include "../include/odbc/odbc.h"

SQLRETURN SQLFreeHandle(  
     SQLSMALLINT   HandleType,  
     SQLHANDLE     handle
)
{
    switch(HandleType) {
        case SQL_HANDLE_STMT:
            free((StatementHandle*) handle);
            break;
        default:
            free((Handle* ) handle);
    }
    return SQL_SUCCESS;
}

