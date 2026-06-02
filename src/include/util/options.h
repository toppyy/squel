#pragma once
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include "../const.h"

typedef enum {
    OPT_HTSIZE,
    OPT_ENABLE_HJ,
    OPT_TDB_ROWGROUP_SIZE
} Option;

typedef struct Options {
    size_t htsize;
    size_t enableHashjoin;
    size_t tdbRowgroupSize;
} Options;

extern Options* OPTIONS;

Options* initOptions();

size_t getOption(Option opt);


