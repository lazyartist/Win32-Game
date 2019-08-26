#include "stdafx.h"
#include "lib.h"
#include "CGameFrame_UnitStatePattern.h"
#include "CGameFrame.cpp"
#include <iostream>


CGameFrame_UnitStatePattern::CGameFrame_UnitStatePattern()
{
	dlog("CGameFrame_UnitStatePattern");
}


CGameFrame_UnitStatePattern::~CGameFrame_UnitStatePattern()
{
	dlog("~CGameFrame_UnitStatePattern");
}


void CGameFrame_UnitStatePattern::InitImpl()
{
	dlog("CGameFrame_UnitStatePattern::InitImpl");

	_xyClientCenter.x = _whClientSize.w / 2;
	_xyClientCenter.y = _whClientSize.h / 2;

	InitUnit();

	IsPlaying = false;
}

void CGameFrame_UnitStatePattern::UpdateLogicImpl()
{
	dlog("CGameFrame_UnitStatePattern::UpdateLogicImpl");

	if (IsPlaying) {

		//UnitState curUnitState = UnitStates[UnitStateIndex];
		UnitState curUnitState = UnitStatePattern.GetCurUnitState();

		// 다음 지점까지의 거리
		fXY distanceXY = curUnitState.XY - _unit.XY;
		float distance = distanceXY.distance();
		float speed = _unit.SpeedPerSeconds * _fDeltaTime;
		if (distance < speed) speed = distance;

		float rad = atan2(distanceXY.y, distanceXY.x);

		float speedX = speed * cos(rad);
		float speedY = speed * sin(rad);

		_unit.XY.x += speedX;
		_unit.XY.y += speedY;

		if (sameXY(curUnitState.XY, _unit.XY)) {
			if (curUnitState.UnitStateType == UnitStateType::Idle) {
				if (_unitIdleTime == 0) {
					_unitIdleTime = GetTickCount();
					return;
				}
				else {
					if (GetTickCount() - _unitIdleTime >= curUnitState.Time) {
						//
					}
					else {
						return;
					}
				}
			}
			else if (curUnitState.UnitStateType == UnitStateType::Walk) {
				//
			}

			_unitIdleTime = 0;

			UnitStatePattern.UpUnitStateIndex();
		}
	}
}

void CGameFrame_UnitStatePattern::UpdateRenderImpl()
{
	dlog("CGameFrame_UnitStatePattern::UpdateRenderImpl");

	// cross line
	HPEN hCrossLInePen = CreatePen(PS_SOLID, 1, RGB(255, 255, 0));
	HPEN hOldCrossLInePen = (HPEN)SelectObject(_hdcMem, hCrossLInePen);
	MoveToEx(_hdcMem, 0, _xyClientCenter.y, nullptr);
	LineTo(_hdcMem, _whClientSize.w, _xyClientCenter.y);
	MoveToEx(_hdcMem, _xyClientCenter.x, 0, nullptr);
	LineTo(_hdcMem, _xyClientCenter.x, _whClientSize.h);
	DeleteObject(SelectObject(_hdcMem, hOldCrossLInePen));

	// unit
	DrawUnitState();
	DrawUnit();
}

void CGameFrame_UnitStatePattern::ReleaseImpl()
{
	dlog("CGameFrame_UnitStatePattern::ReleaseImpl");
}

void CGameFrame_UnitStatePattern::DrawUnit()
{
	HPEN hPen = CreatePen(PS_SOLID, 1, RGB(0, 0, 255));
	HPEN hOldPen = (HPEN)SelectObject(_hdcMem, hPen);

	Ellipse(_hdcMem, _unit.XY.x - _unit.WH.w / 2, _unit.XY.y - _unit.WH.h / 2,
		_unit.XY.x + _unit.WH.w / 2, _unit.XY.y + _unit.WH.w / 2);

	DeleteObject(SelectObject(_hdcMem, hOldPen));
}

void CGameFrame_UnitStatePattern::DrawUnitState()
{
	if (UnitStatePattern.UnitStates.size() == 0) return;

	HPEN hLinePen = (HPEN)GetStockObject(BLACK_PEN);
	HPEN hIdlePen = CreatePen(PS_SOLID, 1, RGB(0, 255, 0));;
	HPEN hWalkPen = CreatePen(PS_SOLID, 1, RGB(0, 0, 255));;
	HPEN hSelectedPen = CreatePen(PS_SOLID, 1, RGB(255, 0, 0));

	HPEN hOldPen = (HPEN)SelectObject(_hdcMem, hWalkPen);

	MoveToEx(_hdcMem, UnitStatePattern.UnitStates[0].XY.x, UnitStatePattern.UnitStates[0].XY.y, nullptr);
	WH wh = { 10, 10 };

	for (size_t i = 0; i < UnitStatePattern.UnitStates.size(); i++)
	{
		UnitState unitState = UnitStatePattern.UnitStates[i];

		SelectObject(_hdcMem, hLinePen);
		LineTo(_hdcMem, unitState.XY.x, unitState.XY.y);

		if (i == SelectedUnitStateIndex) {
			SelectObject(_hdcMem, hSelectedPen);
		}
		else if (unitState.UnitStateType == UnitStateType::Idle) {
			SelectObject(_hdcMem, hIdlePen);
		}
		else if (unitState.UnitStateType == UnitStateType::Walk) {
			SelectObject(_hdcMem, hWalkPen);
		}

		Ellipse(_hdcMem, unitState.XY.x - wh.w / 2, unitState.XY.y - wh.h / 2,
			unitState.XY.x + wh.w / 2, unitState.XY.y + wh.w / 2);

		SelectObject(_hdcMem, hWalkPen);
	}

	SelectObject(_hdcMem, hOldPen);
	DeleteObject(hLinePen);
	DeleteObject(hIdlePen);
	DeleteObject(hWalkPen);
	DeleteObject(hSelectedPen);
}

void CGameFrame_UnitStatePattern::InitUnit()
{
	_unit.SpeedPerSeconds = 500;
	_unit.XY = _xyClientCenter;
	_unit.WH = { 30, 30 };

	UnitStateIndex = 0;
	SelectedUnitStateIndex = NoSelectedIndex;
}

void CGameFrame_UnitStatePattern::PlayStop(bool isPlay)
{
	if (isPlay) {
		if (UnitStatePattern.UnitStates.size() == 0) return;

		IsPlaying = true;
		InitUnit();
	}
	else {
		IsPlaying = false;

	}

}
