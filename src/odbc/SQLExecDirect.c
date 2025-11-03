#include "../include/odbc/odbc.h"



SQLRETURN SQLExecDirect(  
     SQLHSTMT     stmtHandle,  
     SQLCHAR *    StatementText,  
     SQLINTEGER   TextLength __attribute__((unused)))
{
    return _InitQuery(stmtHandle, StatementText);
}