#pragma once
#include "../planner/planner.h"
#include "../executor/executor.h"
#include "../executor/tuple.h"
#include "../executor/tuplebuffer.h"
#include "./join.h"
#include "../util/hashmap.h"


void hashjoinGetTuple(Operator* op, Tuple* tpl);