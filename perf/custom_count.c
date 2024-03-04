#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

#include <stdio.h>

#define BUFFSIZE 1000

typedef enum Datatype {
    DTYPE_STR,
    DTYPE_INT,
    DTYPE_UNDEFINED,
    DTYPE_LONG
} Datatype;

void readMetadata(FILE* f) {

    size_t colcount = 0;

    char dummy[10];

    fread(&dummy,     1, sizeof(u_int32_t),  f);
    fread(&colcount,     1, sizeof(size_t), f);

    int metadataSize = sizeof(u_int32_t) + sizeof(size_t);


    char nameLength = 0;

    for (size_t i = 0; i < colcount; i++) {
        fread(&nameLength, 1, sizeof(char), f);
        fread(&dummy,     sizeof(char),       nameLength, f);
        fread(&dummy,     sizeof(Datatype),   1,          f);
        fread(&dummy,     sizeof(char),       1,          f);

        metadataSize += sizeof(char) + (nameLength * sizeof(char)) + sizeof(Datatype) + sizeof(char); // :(
    }

    printf("MD: %d\n", metadataSize);

}

long buffer[BUFFSIZE];


int main(int argc, char** argv) {

    // 204669335

    FILE* f = fopen(argv[1], "r");
    if (f == NULL) {
        printf("Unable to open the file at path '%s'\n", argv[1]);
        exit(1);
    }
    readMetadata(f);

    size_t bytesRead = 0;
    long sum = 0;

    do {
        bytesRead = fread(&buffer, sizeof(long), BUFFSIZE, f);
        printf("Bytes read %ld\n", bytesRead);
        for (size_t i = 0; i < bytesRead; i+=2) {
            printf("%ld\n", buffer[i]);
            sum += buffer[i];
        }
    } while(bytesRead == BUFFSIZE);
    
    printf("Sum: %ld\n", sum);



    


    fclose(f);

}