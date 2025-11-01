#pragma once
#include <stdio.h>
#include <assert.h>
#include "../const.h"
#include "../parser/parsetree.h"
#include "../planner/planner.h"
#include "../operators/join.h"
#include "../operators/hashjoin.h"
#include "../operators/filter.h"
#include "../operators/scan.h"
#include "../operators/scanTDB.h"
#include "../operators/project.h"
#include "../operators/aggregate.h"
#include "../io/tdb.h"

extern char *buffercache;
extern char *bufferscan;

void execute(Operator* op, void (*tupleHandler)(Tuple* tpl));
void executeCreateTable(Operator* op);
void executeInsert(Operator* op);
void executeExplain(Operator* queryplan);