#pragma once
#include <stdio.h>

#include "../const.h"
#include "../parser/parsetree.h"
#include "../planner/planner.h"
#include "../bufferpool/bufferpool.h"
#include "../operators/join.h"
#include "../operators/filter.h"
#include "../operators/scan.h"
#include "../operators/scanTDB.h"
#include "../operators/project.h"
#include "../operators/aggregate.h"
#include "../io/tdb.h"


extern char *buffercache;
extern char *bufferscan;
extern size_t buffercacheSize;

extern TupleBuffer* tplbuffer;

void execute(Operator* op);
void executeStatement(Node* node);