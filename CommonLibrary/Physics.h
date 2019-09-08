#pragma once
#include "lib.h"

class CUnit;

static class Physics {
public:
	static bool bEnable;
	static vector<CUnit> *cUnits;

	static CUnit * hitTest(const CUnit &cUnit);
};