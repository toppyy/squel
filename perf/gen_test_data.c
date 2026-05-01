#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <time.h>

#define ROWMAX 200

void make_row(char *ptr_row, size_t bufsz, int count) {
    
    int written = 0;
    written = snprintf(ptr_row, bufsz, "\n");

    for (int i = 0; i < count && written < (int)bufsz; ++i) {
        int r = rand();
        int n = snprintf(ptr_row + written, bufsz - written,
                         "%s%d", (i == 0 ? "" : ";"), r);

        if (n < 0 || n >= (int)(bufsz - written)) break;
        written += n;
    }
}


void make_header(char *ptr_row, size_t bufsz, int count) {
    
    int written = 0;
    written = snprintf(ptr_row, bufsz, "");

    for (int i = 0; i < count && written < (int)bufsz; ++i) {
        int r = rand();
        int n = snprintf(ptr_row + written, bufsz - written,
                         "%s%s%d", (i == 0 ? "" : ";"), "col", i);

        if (n < 0 || n >= (int)(bufsz - written)) break;
        written += n;
    }
}



int main(int argc, char** argv) {


    if (argc < 3) {
        printf("You need to provide the target path and number of rows\n");
        exit(1);
    }

    char* path = argv[1];
    long records = atol(argv[2]);
    int  columns = 2;
    if (argc > 2) columns = atoi(argv[3]);


    int fd = open(path, O_RDWR | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR); 
    if (fd == 0) {
        printf("Unable to open the file at path '%s'\n", path);
        exit(1);
    }

    time_t t = time(0);
    srand(t);
    
    
    char row[ROWMAX];
    char* ptr_row = row;
    
    

    make_header(ptr_row, ROWMAX, columns);
    write(fd, ptr_row, strlen(row));


    for (long i = 0; i < records; i++) {
        make_row(ptr_row, ROWMAX, columns);
        write(fd, ptr_row, strlen(row));
    }


    close(fd);
}