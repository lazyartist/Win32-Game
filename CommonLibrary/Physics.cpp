#include "stdafx.h"
#include "Physics.h"
#include "lib.h"

bool Physics::bEnable = false;
vector<CUnit> *Physics::cUnits;

CUnit * Physics::hitTest(const CUnit &cUnit) {
	RECT collision = cUnit.GetCollision();
	//RECT collision = cUnit._cCurSpriteInfo.sLocalCollisions[0];

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