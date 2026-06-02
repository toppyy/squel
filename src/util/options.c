#include "../include/util/options.h"

Options* OPTIONS;

Options* initOptions() {
    OPTIONS = malloc(sizeof(Options));
    OPTIONS->htsize = HTSIZE;
    OPTIONS->enableHashjoin     = HASHJOIN;
    OPTIONS->tdbRowgroupSize    = TDB_ROWGROUP_SIZE;
    return OPTIONS;
}

size_t getOption(Option opt) {
    switch(opt) {
        case OPT_HTSIZE:
            return OPTIONS->htsize;
        case OPT_ENABLE_HJ:
            return OPTIONS->enableHashjoin;
        case OPT_TDB_ROWGROUP_SIZE:
            return OPTIONS->tdbRowgroupSize;
    }

    printf("getOption: Tried to retrieve an unknown option\n");
    exit(1);
}
