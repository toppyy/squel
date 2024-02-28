#include "../include/io/tdb.h"

void writeTdb(char* path, TDB tbldef) {

    FILE* f = fopen(path, "w+");
    if (f == NULL) {
        printf("Unable to open the file at path '%s'\n", path);
        exit(1);
    }

    // Number of records
    fwrite(&tbldef.records, 1, sizeof(tbldef.records), f);

    // Number of columns
    fwrite(&tbldef.colCount, 1, sizeof(tbldef.colCount), f);

    // Columns: Datatype and length (in bytes)
    for (size_t i = 0; i < tbldef.colCount; i++) {
        char nameLength = strlen(tbldef.colNames[i]) + 1;
        fwrite(&nameLength, 1, sizeof(char), f);
        fwrite(&tbldef.colNames[i], nameLength, sizeof(char), f);
        fwrite(&tbldef.datatypes[i], 1, sizeof(Datatype), f);
        fwrite(&tbldef.lengths[i], 1, sizeof(char), f);
    }

    fclose(f);
}

TDB readTdbMetadata(char* path) {

    FILE* f = fopen(path, "r");
    if (f == NULL) {
        printf("Unable to open the file at path '%s'\n", path);
        exit(1);
    }

    TDB tbldef;

    fread(&tbldef.records,  1, sizeof(tbldef.records),  f);
    fread(&tbldef.colCount, 1, sizeof(tbldef.colCount), f);


    char nameLength = 0;
    for (size_t i = 0; i < tbldef.colCount; i++) {
        fread(&nameLength, 1, sizeof(char), f);
        fread(&tbldef.colNames[i],  sizeof(char),       nameLength, f);
        fread(&tbldef.datatypes[i], sizeof(Datatype),   1,          f);
        fread(&tbldef.lengths[i],   sizeof(char),       1,          f);
    }

    fclose(f);

    return tbldef;
}

void buildPathToTDBtable(char* ptr_target, char* tblName) {
    sprintf(ptr_target, "%s/%s.%s", DATAPATH, tblName, TDBFILEXT);
}

