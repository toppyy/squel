#pragma once
#include <stdbool.h>

#define SIZEOF(arr) (sizeof(arr) / sizeof(*arr))

bool isNumeric(char chr);
bool isLetter(char chr);
bool isAlphaNumeric(char chr);
bool isWhiteSpace(char chr);