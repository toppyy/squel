#pragma once
#include "../planner/planner.h"
#include "../executor/executor.h"
#include "../executor/tuple.h"

void scanGetTuple(Operator* op, Tuple* tpl);