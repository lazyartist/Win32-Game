#pragma once
#include "lib.h"

class CUnit;

static class Physics {
public:
	static vector<CUnit> *cUnits;

	static CUnit * hitTest(const CUnit &cUnit);
};