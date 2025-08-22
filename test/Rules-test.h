#pragma once

#include "Rules.h"

bool run_all_rules_tests();
bool test_destinationContainsSamePlayer(const Rules& rules);
bool test_isKingInCheckAfterMove(const Rules& rules);