#pragma once
#include "CGameFramework.h"
#include "CController.h"
#include "lib.h"

class CPhysicsTester :
	public CGameFramework {
public:
	CPhysicsTester();
	virtual ~CPhysicsTester();
	// implement derived class
	virtual void InitImpl();
	virtual void UpdateLogicImpl();
	virtual void UpdateControllerImpl();
	virtual void UpdateRenderImpl();
	virtual void ReleaseImpl();

	void AddForce(SXY v);

	CUnit *cControlUnit;
	SXY sAccle = {0.0, 0.0};
	CController cController;
};

