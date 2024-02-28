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
    printf("Wrote %ld cols\n", tbldef.colCount);

    // Columns: Datatype and length (in bytes)
    for (size_t i = 0; i < tbldef.colCount; i++) {
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

    for (size_t i = 0; i < tbldef.colCount; i++) {
        fread(&tbldef.datatypes[i], 1, sizeof(Datatype),    f);
        fread(&tbldef.lengths[i],   1, sizeof(char),        f);
    }

    fclose(f);

    return tbldef;
}

void buildPathToTDBtable(char* ptr_target, char* tblName) {
    sprintf(ptr_target, "%s/%s.%s", DATAPATH, tblName, TDBFILEXT);
}

