#include "stdafx.h"
#include "CController.h"
#include "lib.h"

CController::CController() {
}
CController::~CController() {
}

//void CController::Init(CUnit * cUnit) {
//	this->cUnit = cUnit;
//}

void CController::Update(float fDeltaTime, CUnit *cUnit) {
	CUnitState cCurUnitState = cUnit->cUnitStateAction.GetCurUnitState();
	if (!cCurUnitState.bCancelable) {
		return;
	}

	CUnitState cUnitState;
	cUnitState.eUnitStateType = EUnitStateType::EUnitStateType_None;
	cUnitState.sXY = { 0.0, 0.0 };
	if (GetAsyncKeyState(VK_LEFT) & 0x8000) {
		cUnitState.eUnitStateType = EUnitStateType::EUnitStateType_MoveTo;
		cUnitState.sXY.Add(-1, 0);
	}
	if (GetAsyncKeyState(VK_RIGHT) & 0x8000) {
		cUnitState.eUnitStateType = EUnitStateType::EUnitStateType_MoveTo;
		cUnitState.sXY.Add(1, 0);
	}
	if (GetAsyncKeyState(VK_UP) & 0x8000) {
		cUnitState.eUnitStateType = EUnitStateType::EUnitStateType_MoveTo;
		cUnitState.sXY.Add(0, -1);
	}
	if (GetAsyncKeyState(VK_DOWN) & 0x8000) {
		cUnitState.eUnitStateType = EUnitStateType::EUnitStateType_MoveTo;
		cUnitState.sXY.Add(0, 1);
	}
	if (GetAsyncKeyState('A') & 0x8000) {
		cUnitState.eUnitStateType = EUnitStateType::EUnitStateType_Shoot;
	}

	if (cUnitState.eUnitStateType == EUnitStateType::EUnitStateType_MoveTo) {
		// 이동 거리로 x, y값을 계산(이렇게 하지 않으면 x, y를 동시에 움직일 경우 더 많은 거리를 이동한다)
		float _fDirectionRadian = atan2(cUnitState.sXY.y, cUnitState.sXY.x);
		cUnitState.sXY = { cos(_fDirectionRadian) , sin(_fDirectionRadian) }; // 각도로 x, y 좌표를 얻었으므로 정규화 되어있다.
		cUnitState.sXY = { cUnitState.sXY.x * cUnit->fSpeedPerSeconds * fDeltaTime * Const::fSpeedPerFrameMagnification(), cUnitState.sXY.y * cUnit->fSpeedPerSeconds * fDeltaTime * Const::fSpeedPerFrameMagnification() };
		cUnitState.sXY.Add(cUnit->sXY.x, cUnit->sXY.y);
		cUnitState.bCancelable = true;
		cUnitState.bOncePlay = false;
		if (cCurUnitState.eUnitStateType != EUnitStateType::EUnitStateType_MoveTo) {
			cUnit->ClearAni(); //현재 이동중이라면 애니메이션을 초기화하지 않고 연속 재생한다.
		}
		cUnit->cUnitStateAction.Clear();
		cUnit->cUnitStateAction.vecCUnitState.push_back(cUnitState);
	}
	else if (cUnitState.eUnitStateType == EUnitStateType::EUnitStateType_Shoot) {
		cUnitState.bCancelable = false;
		cUnitState.bOncePlay = true;
		cUnit->ClearAni();
		cUnit->cUnitStateAction.Clear();
		cUnit->cUnitStateAction.vecCUnitState.push_back(cUnitState);
	}
}
