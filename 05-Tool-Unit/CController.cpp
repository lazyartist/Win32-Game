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
		// ���� ���� �� ���� ��ٸ���
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
		// �̵� �Ÿ��� x, y���� ���(�̷��� ���� ������ x, y�� ���ÿ� ������ ��� �� ���� �Ÿ��� �̵��Ѵ�)
		float _fDirectionRadian = atan2(cUnitState.sXY.y, cUnitState.sXY.x);
		cUnitState.sXY = { cos(_fDirectionRadian) , sin(_fDirectionRadian) }; // ������ x, y ��ǥ�� ������Ƿ� ����ȭ �Ǿ��ִ�.
		cUnitState.sXY = { cUnitState.sXY.x * cUnit->fSpeedPerSeconds * fDeltaTime * Const::fSpeedPerFrameMagnification(), cUnitState.sXY.y * cUnit->fSpeedPerSeconds * fDeltaTime * Const::fSpeedPerFrameMagnification() };
		cUnitState.sXY.Add(cUnit->sXY.x, cUnit->sXY.y);
		if (cCurUnitState.eUnitStateType != EUnitStateType::EUnitStateType_MoveTo) {
			//�̵��߿��� �ִϸ��̼��� ��� ����Ѵ�.
			cUnit->ClearAni();
		}
		cUnit->cUnitStateAction.Clear();
		cUnit->cUnitStateAction.vecCUnitState.push_back(cUnitState);
	}
	else if (cUnitState.eUnitStateType == EUnitStateType::EUnitStateType_Shoot) {
		// shoot
		cUnit->ClearAni();
		cUnit->cUnitStateAction.Clear();
		cUnit->cUnitStateAction.vecCUnitState.push_back(cUnitState);
	}
}
