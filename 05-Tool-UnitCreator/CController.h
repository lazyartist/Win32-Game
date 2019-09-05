#pragma once
#include "lib.h"

class CUnit;
class CController {
public:
	CController();
	~CController();
	//void Init(CUnit *cUnit);
	void Update(float fDeltaTime, CUnit *cUnit);

	CUnit *cUnit;
};

