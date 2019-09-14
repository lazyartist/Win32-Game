#pragma once
#include "lib.h"

class CUnit;

static class Physics {
public:
	static bool bEnable;
	static vector<CUnit> *cUnits;
	static vector<CUnit*> *pUnits;

	static CUnit * TestCollision(const CUnit &cUnit);
	static CUnit * TestRectCollision(const CUnit &cUnit, RECT *rect);
	static void UpdateMove(CUnit &cUnit, const float _fDeltaTime);
};