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

	if (cAction.eActionType == EActionType::EActionType_MoveTo) {
		// �̵� �Ÿ��� x, y���� ���(�̷��� ���� ������ x, y�� ���ÿ� ������ ��� �� ���� �Ÿ��� �̵��Ѵ�)
		float _fDirectionRadian = atan2(cAction.sXY.y, cAction.sXY.x);
		cAction.sXY = { cos(_fDirectionRadian) , sin(_fDirectionRadian) }; // ������ x, y ��ǥ�� ������Ƿ� ����ȭ �Ǿ��ִ�.
		cAction.sXY = { cAction.sXY.x * cUnit->fSpeedPerSeconds * fDeltaTime * Const::fSpeedPerFrameMagnification(), cAction.sXY.y * cUnit->fSpeedPerSeconds * fDeltaTime * Const::fSpeedPerFrameMagnification() };
		cAction.sXY.Add(cUnit->sXY.x, cUnit->sXY.y);
		cAction.bCancelable = true;
		cAction.bOncePlay = false;
		if (cCurAction.eActionType != EActionType::EActionType_MoveTo) {
			cUnit->ClearAni(); //���� �̵����̶�� �ִϸ��̼��� �ʱ�ȭ���� �ʰ� ���� ����Ѵ�.
		}
		cUnit->cActionList.Clear();
		cUnit->cActionList.cActions.push_back(cAction);
	}
	else if (cAction.eActionType == EActionType::EActionType_Shoot) {
		cAction.bCancelable = false;
		cAction.bOncePlay = true;
		cUnit->ClearAni();
		cUnit->cActionList.Clear();
		cUnit->cActionList.cActions.push_back(cAction);
	}
}
