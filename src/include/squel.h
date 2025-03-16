#pragma once


typedef enum {
    OPT_HTSIZE
} Option;

typedef struct Options {
    size_t htsize;
} Options;

extern Options* OPTIONS;

Options* initOptions();

size_t getOption(Option opt);