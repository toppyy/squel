#pragma once

#include <string.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
/* Utilities for interacting with the file system */
#define LINEBUFF        200


bool fileExists(const char* path);

char* readLine(FILE* f);