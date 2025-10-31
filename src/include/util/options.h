#pragma once
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include "../const.h"

typedef enum {
    OPT_HTSIZE,
    OPT_ENABLE_HJ
} Option;

typedef struct Options {
    size_t htsize;
    size_t enableHashjoin; // 1 if hashjoin enabled (default), 0 if not
} Options;

extern Options* OPTIONS;

Options* initOptions();

size_t getOption(Option opt);


