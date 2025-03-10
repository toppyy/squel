#pragma once
#include "../planner/planner.h"
#include "../executor/executor.h"
#include "../executor/tuple.h"
#include "../executor/tuplebuffer.h"
#include <errno.h>

void scanTDBGetTuple(Operator* op, Tuple* tpl);