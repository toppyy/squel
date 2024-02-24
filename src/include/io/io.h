#pragma once

#include <string.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <errno.h>
#include "../const.h"

/* Utilities for interacting with the file system */

bool  fileExists(const char* path);
char* readLine(FILE* f);
char* readLineToBuffer(FILE* f, char* buff, size_t buffSize);