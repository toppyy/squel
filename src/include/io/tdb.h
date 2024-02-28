#pragma once
#include "../parser/parsetree.h"
#include "../const.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

/* 

    tdb is a custom file format. 

    Layout is:
        u64bit int - number of records
        8bit int - number of columns
        At least one 8bit int for each column indicating type
            If it's a string, a second 8bit int indicating length of
        Records..        

*/

typedef struct TDB {
    char alias[CHARMAXSIZE];
    u_int32_t records;
    size_t colCount;
    char lengths[ARRAYMAXSIZE];
    Datatype datatypes[ARRAYMAXSIZE];
} TDB;

void writeTdb(char* path, TDB tbldef);
TDB readTdbMetadata(char* path);
void buildPathToTDBtable(char* ptr_target, char* tblName);