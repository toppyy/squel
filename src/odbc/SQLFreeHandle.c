

#include "../include/odbc/odbc.h"

SQLRETURN SQLFreeHandle(  
     SQLSMALLINT   HandleType,  
     SQLHANDLE     handle
)
{
    switch(HandleType) {
        case SQL_HANDLE_STMT:
            freeQueryplan(((StatementHandle*) handle)->queryplan);
            freeTuple(((StatementHandle*) handle)->tpl);
            free((StatementHandle*) handle);
            break;
        default:
            free((Handle* ) handle);
    }
    return SQL_SUCCESS;
}

