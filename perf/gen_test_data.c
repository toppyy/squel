#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define ROWMAX 50

int main(int argc, char** argv) {


    if (argc < 3) {
        printf("You need to provide the target path and number of rows\n");
        exit(1);
    }

    char* path = argv[1];
    long records = atol(argv[2]);

    FILE* f = fopen(path, "w+");
    if (f == NULL) {
        printf("Unable to open the file at path '%s'\n", path);
        exit(1);
    }

    fwrite("col1;col2\n",11,1,f);

    char row[ROWMAX];
    char* ptr_row = row;
    for (size_t i = 0; i < records; i++) {
        memset(ptr_row,0,ROWMAX);
        sprintf(ptr_row, "%d;%d\n", rand(), rand());
        fwrite(ptr_row, strlen(row), 1, f);
    }


    fclose(f);
}