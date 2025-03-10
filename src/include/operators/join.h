#pragma once
#include "../planner/planner.h"
#include "../executor/executor.h"
#include "../executor/tuple.h"
#include "../executor/tuplebuffer.h"


void joinGetTuple(Operator* op, Tuple* tpl);