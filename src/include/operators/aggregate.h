#pragma once
#include <stdbool.h>
#include "../planner/planner.h"
#include "../executor/executor.h"
#include "../executor/tuple.h"


void aggregateGetTuple(Operator* op, Tuple* tpl);