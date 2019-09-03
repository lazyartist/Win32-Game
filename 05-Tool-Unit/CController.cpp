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
	if (cCurUnitState.eUnitStateType == EUnitStateType::EUnitStateType_Shoot) {
		// 슛이 끝날 때 까지 기다리기
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

	if (cUnitState.eUnitStateType == EUnitStateType::EUnitStateType_None) {
		if (cCurUnitState.eUnitStateType != EUnitStateType::EUnitStateType_Idle) {
			// 아무런 키가 눌리지 않았고 마지막 상태가  Idle이 아니라면 Idle 상태 추가
			auto &endUnitState = cUnit->cUnitStateAction.vecCUnitState.back();
			if (endUnitState.eUnitStateType != EUnitStateType::EUnitStateType_Idle) {
				cUnitState.eUnitStateType = EUnitStateType::EUnitStateType_Idle;
				cUnitState.iTime = INT_MAX;
				cUnit->cUnitStateAction.vecCUnitState.push_back(cUnitState);
			}
		}
	}
	else if (cUnitState.eUnitStateType == EUnitStateType::EUnitStateType_MoveTo) {
		// 이동 거리로 x, y값을 계산(이렇게 하지 않으면 x, y를 동시에 움직일 경우 더 많은 거리를 이동한다)
		float _fDirectionRadian = atan2(cUnitState.sXY.y, cUnitState.sXY.x);
		cUnitState.sXY = { cos(_fDirectionRadian) , sin(_fDirectionRadian) }; // 각도로 x, y 좌표를 얻었으므로 정규화 되어있다.
		cUnitState.sXY = { cUnitState.sXY.x * cUnit->fSpeedPerSeconds * fDeltaTime, cUnitState.sXY.y * cUnit->fSpeedPerSeconds * fDeltaTime };
		cUnitState.sXY.Add(cUnit->sXY.x, cUnit->sXY.y);
		// moveto
		cUnit->cUnitStateAction.Clear();
		cUnit->cUnitStateAction.vecCUnitState.push_back(cUnitState);
	}
	else if (cUnitState.eUnitStateType == EUnitStateType::EUnitStateType_Shoot) {
		// shoot
		cUnit->cUnitStateAction.Clear();
		cUnit->cUnitStateAction.vecCUnitState.push_back(cUnitState);
		//// idle
		cUnitState.eUnitStateType = EUnitStateType::EUnitStateType_Idle;
		cUnitState.iTime = INT_MAX;
		cUnit->cUnitStateAction.vecCUnitState.push_back(cUnitState);
	}
}
