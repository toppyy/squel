#include "../include/odbc/odbc.h"


RETCODE SQL_API SQLDriverConnect(
    SQLHDBC hdbc __attribute__((unused)),
    SQLHWND hwnd __attribute__((unused)),
    SQLCHAR * szConnStrIn __attribute__((unused)),
    SQLSMALLINT cbConnStrIn __attribute__((unused)),
    SQLCHAR * szConnStrOut,
    SQLSMALLINT cbConnStrOutMax __attribute__((unused)),
    SQLSMALLINT * pcbConnStrOut,
    SQLUSMALLINT fDriverCompletion __attribute__((unused))
) {

    printf("connecting to '%s'\n", szConnStrIn);

    memcpy(szConnStrOut,"connected to squel", 19);
    (*pcbConnStrOut) = 19;

    return SQL_SUCCESS;
}