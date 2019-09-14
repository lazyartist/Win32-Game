#pragma once
#include "lib.h"

class CUnit;
class CController {
public:
	CController();
	~CController();
	void Update(float fDeltaTime, CUnit *cUnit);
	void UpdatePhysics(float fDeltaTime, CUnit *cUnit);

	CUnit *cUnit;
};

