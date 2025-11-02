#include "../include/odbc/odbc.h"





RETCODE SQL_API SQLConnect(
        SQLHDBC ConnectionHandle __attribute__((unused)),
        SQLCHAR *ServerName __attribute__((unused)),
        SQLSMALLINT NameLength1 __attribute__((unused)),
        SQLCHAR *UserName __attribute__((unused)),
        SQLSMALLINT NameLength2 __attribute__((unused)),
        SQLCHAR *Authentication __attribute__((unused)),
        SQLSMALLINT NameLength3 __attribute__((unused))
)
{
    printf("squel: connecting to '%s'\n", ServerName);
	return SQL_SUCCESS;
}
