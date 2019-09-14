#include "stdafx.h"
#include "CPhysicsTester.h"
#include "CGameFramework.cpp"
#include "Physics.h"
#include <vector>

CPhysicsTester::CPhysicsTester() {
	vector<CUnit *> *cUnits = new vector<CUnit *>();
	Physics::pUnits = cUnits;
}
CPhysicsTester::~CPhysicsTester() {
	for (size_t i = 0; i < Physics::pUnits->size(); i++) {
		delete (*Physics::pUnits)[i];
	}
	Physics::pUnits->clear();
	delete Physics::pUnits;
}
void CPhysicsTester::InitImpl() {
	//dlog("InitImpl");
	cControlUnit = new CUnit;
	cControlUnit->eUnitType = EUnitType::EUnitType_Unit;
	cControlUnit->sXY = { 100, 550 };
	cControlUnit->sWH = { 50, 50 };
	cControlUnit->fSpeedPerSeconds = 200;
	cControlUnit->fJumpSpeedPerSeconds = 400;

	CUnit *cGroundUnit = new CUnit();
	cGroundUnit->eUnitType = EUnitType::EUnitType_Ground;
	cGroundUnit->sXY = { -500, 600 };
	cGroundUnit->sWH = { 5000, 50 };
	CUnit *cGroundUnit2 = new CUnit();
	cGroundUnit2->eUnitType = EUnitType::EUnitType_Ground;
	cGroundUnit2->sXY = { 400, 600 };
	cGroundUnit2->sWH = { 300, 50 };
	CUnit *cGroundUnit3 = new CUnit();
	cGroundUnit3->eUnitType = EUnitType::EUnitType_Ground;
	cGroundUnit3->sXY = { 100, 480 };
	cGroundUnit3->sWH = { 200, 30 };
	CUnit *cGroundUnit4 = new CUnit();
	cGroundUnit4->eUnitType = EUnitType::EUnitType_Ground;
	cGroundUnit4->sXY = { 400, 480 };
	cGroundUnit4->sWH = { 200, 30 };

	vector<CUnit *> *cUnits = new vector<CUnit *>();
	cUnits->push_back(cControlUnit);
	cUnits->push_back(cGroundUnit);
	cUnits->push_back(cGroundUnit2);
	cUnits->push_back(cGroundUnit3);
	cUnits->push_back(cGroundUnit4);

	Physics::pUnits = cUnits;
}
void CPhysicsTester::UpdateLogicImpl() {
	cControlUnit->Update(_fDeltaTime);
	Physics::UpdateMove(*cControlUnit, _fDeltaTime);
	//dlog("UpdateLogicImpl");
}
void CPhysicsTester::UpdateControllerImpl() {
	cController.UpdatePhysics(_fDeltaTime, cControlUnit);
	//dlog("UpdateControllerImpl");
}
void CPhysicsTester::UpdateRenderImpl() {
	//draw unit
	HPEN hPen = CreatePen(PS_SOLID, 1, RGB(0, 0, 255));
	HPEN hOldPen = (HPEN)SelectObject(_hdcMem, hPen);
	for (size_t i = 0; i < Physics::pUnits->size(); i++) {
		CUnit *cUnit = (*Physics::pUnits)[i];
		Rectangle(_hdcMem, cUnit->sXY.x, cUnit->sXY.y - cUnit->sWH.h, cUnit->sXY.x + cUnit->sWH.w, cUnit->sXY.y);

		DeleteObject(SelectObject(_hdcMem, hOldPen));
	}
	//dlog("UpdateRenderImpl");
}
void CPhysicsTester::ReleaseImpl() {
	//dlog("ReleaseImpl");
}

void CPhysicsTester::AddForce(SXY v) {
	//sAccle.Add(3.0, -10.0);
	sAccle.Add(v.x, v.y);
}
