#include "../include/odbc/odbc.h"


SQLRETURN SQLGetConnectAttr(  
     SQLHDBC        ConnectionHandle __attribute__((unused)),
     SQLINTEGER     Attribute __attribute__((unused)),
     SQLPOINTER     ValuePtr __attribute__((unused)),
     SQLINTEGER     BufferLength __attribute__((unused)),
     SQLINTEGER *   StringLengthPtr __attribute__((unused))
)
{

     return SQL_SUCCESS;
}