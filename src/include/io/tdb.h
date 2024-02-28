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
        For each column:
            Byte representing column name length
            x bytes for column name
            Byte for column datatype
            Byte for column datalength
        Records..        

*/

typedef struct TDB {
    char alias[CHARMAXSIZE];
    u_int32_t records;
    size_t colCount;
    char lengths[ARRAYMAXSIZE];
    Datatype datatypes[ARRAYMAXSIZE];
    char colNames[ARRAYMAXSIZE][CHARMAXSIZE];
    char path[CHARMAXSIZE];
} TDB;

size_t writeTdbMetadata(char* path, TDB tbldef);
size_t writeTdbMetadataToFD(FILE* f, TDB tbldef);
TDB readTdbMetadata(char* path);
TDB readTdbMetadaFromFD(FILE* f);
void buildPathToTDBtable(char* ptr_target, char* tblName);
