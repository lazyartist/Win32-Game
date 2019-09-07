#include "stdafx.h"
#include "lib.h"
#include "CActionPattern.h"
#include "CGameFramework.cpp"
#include <iostream>

CActionPattern::CActionPattern() {
	dlog("CActionPattern");
}
CActionPattern::~CActionPattern() {
	dlog("~CActionPattern");
}
void CActionPattern::InitImpl() {
	dlog("CActionPattern::InitImpl");

	_xyClientCenter.x = _whClientSize.w / 2;
	_xyClientCenter.y = _whClientSize.h / 2;

	InitUnit();

	IsPlaying = false;
}
void CActionPattern::UpdateLogicImpl() {
	dlog("CActionPattern::UpdateLogicImpl");

	if (IsPlaying) {

		CAction cAction = cActionList.GetCurAction();

		// 다음 지점까지의 거리
		SXY distanceXY = cAction.sXY - _unit.sXY;
		float distance = distanceXY.distance();
		float speed = _unit.fSpeedPerSeconds * _fDeltaTime;
		if (distance < speed) speed = distance;

		float rad = atan2(distanceXY.y, distanceXY.x);

		float speedX = speed * cos(rad);
		float speedY = speed * sin(rad);

		_unit.sXY.x += speedX;
		_unit.sXY.y += speedY;

		if (sameXY(cAction.sXY, _unit.sXY)) {
			if (cAction.eActionType == EActionType::EActionType_Idle) {
				if (_unitIdleTime == 0) {
					_unitIdleTime = GetTickCount();
					return;
				}
				else {
					if (GetTickCount() - _unitIdleTime >= cAction.iTime) {
						//
					}
					else {
						return;
					}
				}
			}
			else if (cAction.eActionType == EActionType::EActionType_MoveTo) {
				//
			}

			_unitIdleTime = 0;

			cActionList.NextAction();
		}
	}
}
void CActionPattern::UpdateRenderImpl() {
	dlog("CActionPattern::UpdateRenderImpl");

	// cross line
	HPEN hCrossLInePen = CreatePen(PS_SOLID, 1, RGB(255, 255, 0));
	HPEN hOldCrossLInePen = (HPEN)SelectObject(_hdcMem, hCrossLInePen);
	MoveToEx(_hdcMem, 0, _xyClientCenter.y, nullptr);
	LineTo(_hdcMem, _whClientSize.w, _xyClientCenter.y);
	MoveToEx(_hdcMem, _xyClientCenter.x, 0, nullptr);
	LineTo(_hdcMem, _xyClientCenter.x, _whClientSize.h);
	DeleteObject(SelectObject(_hdcMem, hOldCrossLInePen));

	// unit
	DrawAction();
	DrawUnit();
}
void CActionPattern::ReleaseImpl() {
	dlog("CActionPattern::ReleaseImpl");
}
void CActionPattern::DrawUnit() {
	HPEN hPen = CreatePen(PS_SOLID, 1, RGB(0, 0, 255));
	HPEN hOldPen = (HPEN)SelectObject(_hdcMem, hPen);

	Ellipse(_hdcMem, _unit.sXY.x - _unit.sWH.w / 2, _unit.sXY.y - _unit.sWH.h / 2,
		_unit.sXY.x + _unit.sWH.w / 2, _unit.sXY.y + _unit.sWH.w / 2);

	DeleteObject(SelectObject(_hdcMem, hOldPen));
}
void CActionPattern::DrawAction() {
	if (cActionList.cActions.size() == 0) return;

	cActionList.RenderActions(_hdcMem);
}
void CActionPattern::InitUnit() {
	_unit.fSpeedPerSeconds = 500;
	_unit.sXY = _xyClientCenter;
	_unit.sWH = { 30, 30 };

	iSelectedActionIndex = NoSelectedIndex;
}
void CActionPattern::PlayStop(bool isPlay) {
	if (isPlay) {
		if (cActionList.cActions.size() == 0) return;

		IsPlaying = true;
		InitUnit();
	}
	else {
		IsPlaying = false;

	}

}
