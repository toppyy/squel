#include "../include/odbc/odbc.h"

SQLRETURN SQLGetDiagRec(  
     SQLSMALLINT     HandleType __attribute__((unused)),
     SQLHANDLE       handle __attribute__((unused)),
     SQLSMALLINT     RecNumber __attribute__((unused)),
     SQLCHAR *       SQLState __attribute__((unused)),
     SQLINTEGER *    NativeErrorPtr __attribute__((unused)),
     SQLCHAR *       MessageText __attribute__((unused)),
     SQLSMALLINT     BufferLength __attribute__((unused)),
     SQLSMALLINT *   TextLengthPtr __attribute__((unused))
)
{
    return SQL_SUCCESS;
}