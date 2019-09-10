#include "stdafx.h"
#include "CController.h"
#include "lib.h"

CController::CController() {
}
CController::~CController() {
}

//void CController::Init(CUnit * pControlUnit) {
//	this->pControlUnit = pControlUnit;
//}

void CController::Update(float fDeltaTime, CUnit *cUnit) {
	CAction cCurAction = cUnit->cActionList.GetCurAction();
	if (!cCurAction.bCancelable) {
		return;
	}

	CAction cAction;
	cAction.eActionType = EActionType::EActionType_None;
	cAction.sXY = { 0.0, 0.0 };
	if (GetAsyncKeyState(VK_LEFT) & 0x8000) {
		cAction.eActionType = EActionType::EActionType_MoveTo;
		cAction.sXY.Add(-1, 0);
	}
	if (GetAsyncKeyState(VK_RIGHT) & 0x8000) {
		cAction.eActionType = EActionType::EActionType_MoveTo;
		cAction.sXY.Add(1, 0);
	}
	if (GetAsyncKeyState(VK_UP) & 0x8000) {
		cAction.eActionType = EActionType::EActionType_MoveTo;
		cAction.sXY.Add(0, -1);
	}
	if (GetAsyncKeyState(VK_DOWN) & 0x8000) {
		cAction.eActionType = EActionType::EActionType_MoveTo;
		cAction.sXY.Add(0, 1);
	}
	if (GetAsyncKeyState('A') & 0x8000) {
		cAction.eActionType = EActionType::EActionType_Shoot;
	}

	if (cAction.eActionType != EActionType::EActionType_None) {
		if (cAction.eActionType == EActionType::EActionType_MoveTo) {
			CActionFactory::MoveTo(*cUnit, &cAction, fDeltaTime, true);
		}
		else if (cAction.eActionType == EActionType::EActionType_Shoot) {
			CActionFactory::Shoot(*cUnit, &cAction);
		}
		cUnit->AddAction(cAction);
	}
}
