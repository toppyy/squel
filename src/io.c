#include "./include/io/io.h"

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
    rtrn[strlen(buff)-1] = '\0';
    return rtrn;

}