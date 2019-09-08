#include "stdafx.h"
#include "Physics.h"
#include "lib.h"

vector<CUnit> *Physics::cUnits;

CUnit * Physics::hitTest(const CUnit &cUnit) {
	RECT collision = cUnit._cCurSpriteInfo.sLocalCollisions[0];

	for (size_t i = 0; i < cUnits->size(); i++) {
		CUnit *pUnit = &(*cUnits)[i];
		if (&cUnit != pUnit) {
			for (size_t ii = 0; ii < pUnit->_cCurSpriteInfo.iCollisionCount; ii++) {
				pUnit->_cCurSpriteInfo.sLocalCollisions[ii];
			}
		}
	}
	return nullptr;
}