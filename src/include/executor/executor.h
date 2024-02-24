#pragma once
#include <stdio.h>
#include "../const.h"
#include "../parser/parsetree.h"
#include "../planner/planner.h"
#include "../buffer/buffer.h"
#include "../operators/join.h"
#include "../operators/filter.h"
#include "../operators/scan.h"
#include "../operators/project.h"
#include "../operators/aggregate.h"


extern char *buffercache;
extern char *bufferscan;
extern size_t buffercacheSize;

extern TupleBuffer* tplbuffer;
void execute(Operator* op);