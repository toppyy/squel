#pragma once
#include "../parser/parsetree.h"
#include "../planner/planner.h"
#include "../buffer/buffer.h"
#include "../operators/join.h"
#include "../operators/filter.h"
#include "../operators/scan.h"
#include "../operators/project.h"

#define QUERYBUFFER 1000
#define SCANBUFFER  1000
#define DELIMITER   ';'
#define DELIMITERSTR ";"


extern char *buffercache;
extern char *bufferscan;
extern size_t buffercacheSize;

extern TupleBuffer* tplbuffer;
void execute(Operator* op);