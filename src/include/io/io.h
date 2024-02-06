#pragma once

#include <string.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <errno.h>

/* Utilities for interacting with the file system */
#define LINEBUFF        200
#define LF              10

bool fileExists(const char* path);

char* readLine(FILE* f);


char* readLineToBuffer(FILE* f, char* buff, size_t buffSize);