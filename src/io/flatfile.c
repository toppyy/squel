#include "../include/io/flatfile.h"

bool fileExists(const char* path) {
    return access(path, F_OK) == 0;
}


char* readLine(FILE* f) {
    char* buff = calloc(LINEBUFF, sizeof(char));

    char* rtrn = fgets(buff, LINEBUFF, f);
    if (strlen(buff) >= LINEBUFF) {
        printf("Error: Tried to read a line that could not fit into the buffer of size %d.\n", LINEBUFF);
        exit(1);
    }
    // Remove LF
    if (rtrn[strlen(buff)-1] == LF) rtrn[strlen(buff)-1] = '\0';
    return rtrn;

}


char* readLineToBuffer(FILE* f, char* buff, size_t buffSize) {

    char* rtrn = fgets(buff, buffSize, f);

    if (rtrn == NULL) {
        return NULL;
    }
    if (strlen(buff) >= buffSize) {
        printf("Error: Tried to read a line that could not fit into the buffer of size %ld.\n", buffSize);
        exit(1);
    }
    // Remove LF
    if (rtrn[strlen(buff)-1] == LF) rtrn[strlen(buff)-1] = '\0';
    return rtrn;
    
}