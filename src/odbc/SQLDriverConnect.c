#include "../include/odbc/odbc.h"

SQLRETURN SQLDriverConnect(  
     SQLHDBC         ConnectionHandle __attribute__((unused)),
     SQLHWND         WindowHandle __attribute__((unused)),
     SQLCHAR *       InConnectionString __attribute__((unused)),
     SQLSMALLINT     StringLength1 __attribute__((unused)),
     SQLCHAR *       OutConnectionString __attribute__((unused)),
     SQLSMALLINT     BufferLength __attribute__((unused)),
     SQLSMALLINT *   StringLength2Ptr __attribute__((unused)),
     SQLUSMALLINT    DriverCompletion __attribute__((unused))
)
{
    return SQL_SUCCESS;
}