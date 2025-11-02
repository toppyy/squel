#include "../include/odbc/odbc.h"


SQLRETURN SQLDisconnect(  
    SQLHDBC  ConnectionHandle __attribute__((unused))
)
{
    // There is no real work to do as this is connectionless
    return SQL_SUCCESS;
}

