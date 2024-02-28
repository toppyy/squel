#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>


#define ROWMAX 50

int main(int argc, char** argv) {


    if (argc < 3) {
        printf("You need to provide the target path and number of rows\n");
        exit(1);
    }

    char* path = argv[1];
    long records = atol(argv[2]);


    int fd = open(path, O_RDWR | O_CREAT | O_TRUNC);
    if (fd == 0) {
        printf("Unable to open the file at path '%s'\n", path);
        exit(1);
    }

    write(fd, "col1;col2",9 );

    char row[ROWMAX];
    char* ptr_row = row;
    for (long i = 0; i < records; i++) {
        memset(ptr_row,0,ROWMAX);
        sprintf(ptr_row, "\n%d;%d", rand(), rand());
        write(fd, ptr_row, strlen(row));
    }


    close(fd);
}