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

		//CUnitState curUnitState = vecCUnitState[iUnitStateIndex];
		CUnitState curUnitState = UnitStatePattern.GetCurUnitState();

		// 다음 지점까지의 거리
		SXY distanceXY = curUnitState.sXY - _unit.sXY;
		float distance = distanceXY.distance();
		float speed = _unit.fSpeedPerSeconds * _fDeltaTime;
		if (distance < speed) speed = distance;

		float rad = atan2(distanceXY.y, distanceXY.x);

		float speedX = speed * cos(rad);
		float speedY = speed * sin(rad);

		_unit.sXY.x += speedX;
		_unit.sXY.y += speedY;

		if (sameXY(curUnitState.sXY, _unit.sXY)) {
			if (curUnitState.eUnitStateType == EUnitStateType::Idle) {
				if (_unitIdleTime == 0) {
					_unitIdleTime = GetTickCount();
					return;
				}
				else {
					if (GetTickCount() - _unitIdleTime >= curUnitState.iTime) {
						//
					}
					else {
						return;
					}
				}
			}
			else if (curUnitState.eUnitStateType == EUnitStateType::EUnitStateType_MoveTo) {
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

	Ellipse(_hdcMem, _unit.sXY.x - _unit.sWH.w / 2, _unit.sXY.y - _unit.sWH.h / 2,
		_unit.sXY.x + _unit.sWH.w / 2, _unit.sXY.y + _unit.sWH.w / 2);

	DeleteObject(SelectObject(_hdcMem, hOldPen));
}

void CGameFrame_UnitStatePattern::DrawUnitState()
{
	if (UnitStatePattern.vecCUnitState.size() == 0) return;

	UnitStatePattern.RenderUnitState(_hdcMem);

	/*HPEN hLinePen = (HPEN)GetStockObject(BLACK_PEN);
	HPEN hIdlePen = CreatePen(PS_SOLID, 1, RGB(0, 255, 0));;
	HPEN hWalkPen = CreatePen(PS_SOLID, 1, RGB(0, 0, 255));;
	HPEN hSelectedPen = CreatePen(PS_SOLID, 1, RGB(255, 0, 0));

	HPEN hOldPen = (HPEN)SelectObject(_hdcMem, hWalkPen);

	MoveToEx(_hdcMem, CUnitStatePattern.vecCUnitState[0].sXY.x, CUnitStatePattern.vecCUnitState[0].sXY.y, nullptr);
	sWH wh = { 10, 10 };

	for (size_t i = 0; i < CUnitStatePattern.vecCUnitState.size(); i++)
	{
		CUnitState unitState = CUnitStatePattern.vecCUnitState[i];

		SelectObject(_hdcMem, hLinePen);
		LineTo(_hdcMem, unitState.sXY.x, unitState.sXY.y);

		if (i == SelectedUnitStateIndex) {
			SelectObject(_hdcMem, hSelectedPen);
		}
		else if (unitState.EUnitStateType == EUnitStateType::Idle) {
			SelectObject(_hdcMem, hIdlePen);
		}
		else if (unitState.EUnitStateType == EUnitStateType::EUnitStateType_MoveTo) {
			SelectObject(_hdcMem, hWalkPen);
		}

		Ellipse(_hdcMem, unitState.sXY.x - wh.w / 2, unitState.sXY.y - wh.h / 2,
			unitState.sXY.x + wh.w / 2, unitState.sXY.y + wh.w / 2);

		SelectObject(_hdcMem, hWalkPen);
	}

	SelectObject(_hdcMem, hOldPen);
	DeleteObject(hLinePen);
	DeleteObject(hIdlePen);
	DeleteObject(hWalkPen);
	DeleteObject(hSelectedPen);*/
}

void CGameFrame_UnitStatePattern::InitUnit()
{
	_unit.fSpeedPerSeconds = 500;
	_unit.sXY = _xyClientCenter;
	_unit.sWH = { 30, 30 };

	UnitStateIndex = 0;
	SelectedUnitStateIndex = NoSelectedIndex;
}

void CGameFrame_UnitStatePattern::PlayStop(bool isPlay)
{
	if (isPlay) {
		if (UnitStatePattern.vecCUnitState.size() == 0) return;

		IsPlaying = true;
		InitUnit();
	}
	else {
		IsPlaying = false;

	}

}
