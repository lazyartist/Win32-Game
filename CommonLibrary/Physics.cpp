#include "stdafx.h"
#include "Physics.h"
#include "lib.h"

bool Physics::bEnable = false;
vector<CUnit> *Physics::cUnits;
vector<CUnit*> *Physics::pUnits;

CUnit * Physics::TestCollision(const CUnit &cUnit) {
	RECT collision = cUnit.GetCollision();
	for (size_t i = 0; i < cUnits->size(); i++) {
		CUnit *pOtherUnit = &(*cUnits)[i];
		if (&cUnit != pOtherUnit) {
			for (size_t ii = 0; ii < pOtherUnit->_cCurSpriteInfo.iCollisionCount; ii++) {
				RECT otherCollision = pOtherUnit->GetCollision();
				RECT resultRect;
				if (IntersectRect(&resultRect, &collision, &otherCollision) != 0) {
					return pOtherUnit;
				};
			}
		}
	}
	return nullptr;
}
CUnit * Physics::TestRectCollision(const CUnit &cUnit, RECT *rect) {
	RECT collision = { cUnit.sXY.x, cUnit.sXY.y - cUnit.sWH.h, cUnit.sXY.x + cUnit.sWH.w, cUnit.sXY.y };
	for (size_t i = 0; i < pUnits->size(); i++) {
		CUnit *pOtherUnit = (*pUnits)[i];
		if (&cUnit != pOtherUnit) {
			RECT otherCollision = { pOtherUnit->sXY.x, pOtherUnit->sXY.y - pOtherUnit->sWH.h, pOtherUnit->sXY.x + pOtherUnit->sWH.w, pOtherUnit->sXY.y };
			if (IntersectRect(rect, &collision, &otherCollision) != 0) {
				return pOtherUnit;
			};
		}
	}
	return nullptr;
}

void Physics::UpdateMove(CUnit & cUnit, const float _fDeltaTime) {
	//가속도 적용
	SXY sAccel = cUnit.sAccel;
	cUnit.sXY.Add(cUnit.sAccel.x * _fDeltaTime, cUnit.sAccel.y * _fDeltaTime);
	//가속도 감소
	cUnit.sAccel.Add(cUnit.sAccel.x * -14 * _fDeltaTime, Const::fGravity() * _fDeltaTime);
	//충돌
	cUnit.bGround = false;
	RECT rCollision;
	CUnit *cCollisionUnit = TestRectCollision(cUnit, &rCollision);
	if (cCollisionUnit != nullptr && cCollisionUnit->eUnitType == EUnitType_Ground) {
		SXY unitCenter = { cUnit.sXY.x + (float)cUnit.sWH.w / 2.0, cUnit.sXY.y - (float)cUnit.sWH.h / 2.0 };
		SXY collisionCenter = { rCollision.left + (rCollision.right - rCollision.left) / 2.0, rCollision.top + (rCollision.bottom - rCollision.top) / 2.0 };
		float rad = atan2(collisionCenter.y - unitCenter.y, collisionCenter.x - unitCenter.x);
		//dlog("rad", rad);
		if (-Const::fPI14() >= rad && rad >= -Const::fPI34()) {
			//상
			if (sAccel.y <= 0.0) {
				cUnit.sXY.y = cCollisionUnit->sXY.y + cUnit.sWH.h;
				cUnit.sAccel = { cUnit.sAccel.x, 0.0 };
			}
			//dlog("top");
			return;
		}
		else if (Const::fPI14() <= rad && rad <= Const::fPI34()) {
			//하
			if (sAccel.y >= 0.0) {
				cUnit.sXY.y = cCollisionUnit->sXY.y - cCollisionUnit->sWH.h;
				cUnit.sAccel = { cUnit.sAccel.x, 0.0 };
				cUnit.bGround = true;
			}
			//dlog("bottom");
			return;
		}
		else if (Const::fPI34() < rad || rad < -Const::fPI34()) {
			//좌
			cUnit.sXY.x = cCollisionUnit->sXY.x + cCollisionUnit->sWH.w;
			cUnit.sAccel = { 0.0, cUnit.sAccel.y };
			//dlog("left");
			return;
		}
		else if (Const::fPI14() > rad || rad > -Const::fPI14()) {
			//우
			cUnit.sXY.x = cCollisionUnit->sXY.x - cUnit.sWH.w;
			cUnit.sAccel = { 0.0, cUnit.sAccel.y };
			//dlog("right");
			return;
		}
		else {
			//dlog("?");
		}
	}
}
