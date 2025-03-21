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

void execute(Operator* op, bool printColNames, void (*tupleHandler)(Tuple* tpl));
void executeStatement(Node* node);
void executeCreateTable(Node* node);
void executeInsert(Node* node);
void executeExplain(Node* node);