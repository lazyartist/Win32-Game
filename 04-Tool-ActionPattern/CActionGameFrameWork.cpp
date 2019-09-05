#include "stdafx.h"
#include "lib.h"
#include "CActionPatternGameFrameWork.h"
#include "CGameFrame.cpp"
#include <iostream>

CActionPatternGameFrameWork::CActionPatternGameFrameWork() {
	dlog("CActionPatternGameFrameWork");
}
CActionPatternGameFrameWork::~CActionPatternGameFrameWork() {
	dlog("~CActionPatternGameFrameWork");
}
void CActionPatternGameFrameWork::InitImpl() {
	dlog("CActionPatternGameFrameWork::InitImpl");

	_xyClientCenter.x = _whClientSize.w / 2;
	_xyClientCenter.y = _whClientSize.h / 2;

	InitUnit();

	IsPlaying = false;
}
void CActionPatternGameFrameWork::UpdateLogicImpl() {
	dlog("CActionPatternGameFrameWork::UpdateLogicImpl");

	if (IsPlaying) {

		CAction cAction = cActions.GetCurAction();

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

			cActions.NextAction();
		}
	}
}
void CActionPatternGameFrameWork::UpdateRenderImpl() {
	dlog("CActionPatternGameFrameWork::UpdateRenderImpl");

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
void CActionPatternGameFrameWork::ReleaseImpl() {
	dlog("CActionPatternGameFrameWork::ReleaseImpl");
}
void CActionPatternGameFrameWork::DrawUnit() {
	HPEN hPen = CreatePen(PS_SOLID, 1, RGB(0, 0, 255));
	HPEN hOldPen = (HPEN)SelectObject(_hdcMem, hPen);

	Ellipse(_hdcMem, _unit.sXY.x - _unit.sWH.w / 2, _unit.sXY.y - _unit.sWH.h / 2,
		_unit.sXY.x + _unit.sWH.w / 2, _unit.sXY.y + _unit.sWH.w / 2);

	DeleteObject(SelectObject(_hdcMem, hOldPen));
}
void CActionPatternGameFrameWork::DrawAction() {
	if (cActions.deqActions.size() == 0) return;

	cActions.RenderActions(_hdcMem);
}
void CActionPatternGameFrameWork::InitUnit() {
	_unit.fSpeedPerSeconds = 500;
	_unit.sXY = _xyClientCenter;
	_unit.sWH = { 30, 30 };

	iSelectedActionIndex = NoSelectedIndex;
}
void CActionPatternGameFrameWork::PlayStop(bool isPlay) {
	if (isPlay) {
		if (cActions.deqActions.size() == 0) return;

		IsPlaying = true;
		InitUnit();
	}
	else {
		IsPlaying = false;

	}

}
