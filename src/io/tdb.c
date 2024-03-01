#include "../include/io/tdb.h"



size_t writeTdbMetadataToFD(FILE* f, TDB tbldef) {

        
    // Number of records
    fwrite(&tbldef.records, 1, sizeof(tbldef.records), f);
    size_t bytesWritten = sizeof(tbldef.records);

    // Number of columns
    fwrite(&tbldef.colCount, 1, sizeof(tbldef.colCount), f);
    bytesWritten += sizeof(tbldef.records);

    // Columns: Datatype and length (in bytes)
    for (size_t i = 0; i < tbldef.colCount; i++) {
        char nameLength = strlen(tbldef.colNames[i]) + 1;
        fwrite(&nameLength, 1, sizeof(char), f);
        fwrite(&tbldef.colNames[i], nameLength, sizeof(char), f);
        fwrite(&tbldef.datatypes[i], 1, sizeof(Datatype), f);
        fwrite(&tbldef.lengths[i], 1, sizeof(char), f);

        bytesWritten += sizeof(char) + (nameLength * sizeof(char)) + sizeof(Datatype) + sizeof(char); // :(
    }

    return  bytesWritten;
}

size_t writeTdbMetadata(char* path, TDB tbldef) {

    FILE* f = fopen(path, "w+");
    if (f == NULL) {
        printf("Unable to open the file at path '%s'\n", path);
        exit(1);
    }

    size_t bytesWritten = writeTdbMetadataToFD(f, tbldef);

    fclose(f);

    return bytesWritten;
}



TDB readTdbMetadata(char* path) {

    FILE* f = fopen(path, "r");
    if (f == NULL) {
        printf("Unable to open the file at path '%s'\n", path);
        exit(1);
    }
    TDB tbldef = readTdbMetadaFromFD(f);
    strcpy(tbldef.path, path);
    return tbldef;
}

TDB readTdbMetadaFromFD(FILE* f) {


    TDB tbldef;

    fread(&tbldef.records,  1, sizeof(tbldef.records),  f);
    fread(&tbldef.colCount, 1, sizeof(tbldef.colCount), f);

    tbldef.metadataSize = sizeof(tbldef.records) + sizeof(tbldef.colCount);


    char nameLength = 0;

    for (size_t i = 0; i < tbldef.colCount; i++) {
        fread(&nameLength, 1, sizeof(char), f);
        fread(&tbldef.colNames[i],  sizeof(char),       nameLength, f);
        fread(&tbldef.datatypes[i], sizeof(Datatype),   1,          f);
        fread(&tbldef.lengths[i],   sizeof(char),       1,          f);

        tbldef.metadataSize += sizeof(char) + (nameLength * sizeof(char)) + sizeof(Datatype) + sizeof(char); // :(
    }

    fclose(f);

    return tbldef;
}

void buildPathToTDBtable(char* ptr_target, char* tblName) {
    sprintf(ptr_target, "%s/%s.%s", DATAPATH, tblName, TDBFILEXT);
}
