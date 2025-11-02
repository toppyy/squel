#include "../include/odbc/odbc.h"


RETCODE SQL_API SQLAllocHandle(SQLSMALLINT HandleType, SQLHANDLE InputHandle __attribute__((unused)), SQLHANDLE* OutputHandle) {


    printf("SQLAllocHandle %d\n", HandleType);

    switch(HandleType) {
        case SQL_HANDLE_STMT:
            StatementHandle* stmthandle = calloc(1, sizeof(StatementHandle));
            stmthandle->calls = 1;
            stmthandle->type = HandleType;
            *OutputHandle = stmthandle;

            break;

        case SQL_HANDLE_DESC:
            return SQL_ERROR;

        default:
            Handle* handle = calloc(1, sizeof(Handle));
            handle->type = HandleType;
            *OutputHandle = handle;


    }
    return SQL_SUCCESS;
}