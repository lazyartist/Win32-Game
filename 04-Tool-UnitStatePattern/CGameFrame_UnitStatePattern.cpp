#include "stdafx.h"
#include "lib.h"
#include "CGameFrame_UnitStatePattern.h"
#include "CGameFrame.cpp"


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

		UnitState curUnitState = UnitStates[UnitStateIndex];

		// 다음 지점까지의 거리
		fXY distanceXY = curUnitState.xy - _unit.xy;
		float distance = distanceXY.distance();
		float speed = _unit.speedPerSeconds * _fDeltaTime;
		if (distance < speed) speed = distance;

		float rad = atan2(distanceXY.y, distanceXY.x);

		float speedX = speed * cos(rad);
		float speedY = speed * sin(rad);

		_unit.xy.x += speedX;
		_unit.xy.y += speedY;

		if (same(curUnitState.xy, _unit.xy)) {
			UnitStateIndex++;

			if (UnitStateIndex == UnitStates.size()) {
				UnitStateIndex = 0;
				//InitUnit();
			}
		}
	}
}

void CGameFrame_UnitStatePattern::UpdateRenderImpl()
{
	dlog("CGameFrame_UnitStatePattern::UpdateRenderImpl");

	// cross line
	MoveToEx(_hdcMem, 0, _xyClientCenter.y, nullptr);
	LineTo(_hdcMem, _whClientSize.w, _xyClientCenter.y);
	MoveToEx(_hdcMem, _xyClientCenter.x, 0, nullptr);
	LineTo(_hdcMem, _xyClientCenter.x, _whClientSize.h);

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
	Ellipse(_hdcMem, _unit.xy.x - _unit.wh.w / 2, _unit.xy.y - _unit.wh.h / 2,
		_unit.xy.x + _unit.wh.w / 2, _unit.xy.y + _unit.wh.w / 2);
}

void CGameFrame_UnitStatePattern::DrawUnitState()
{
	if (UnitStates.size() == 0) return;

	MoveToEx(_hdcMem, UnitStates[0].xy.x, UnitStates[0].xy.y, nullptr);
	WH wh = {10, 10};

	for (size_t i = 0; i < UnitStates.size(); i++)
	{
		UnitState unitState = UnitStates[i];
		Ellipse(_hdcMem, unitState.xy.x - wh.w / 2, unitState.xy.y - wh.h / 2,
			unitState.xy.x + wh.w / 2, unitState.xy.y + wh.w / 2);

		LineTo(_hdcMem, unitState.xy.x, unitState.xy.y);
	}
}

void CGameFrame_UnitStatePattern::InitUnit()
{
	_unit.speedPerSeconds = 500;
	_unit.xy = _xyClientCenter;
	_unit.wh = { 30, 30 };

	UnitStateIndex = 0;
}

void CGameFrame_UnitStatePattern::PlayStop(bool isPlay)
{
	if (isPlay) {
		if (UnitStates.size() == 0) return;

		IsPlaying = true;
		InitUnit();
	}
	else {
		IsPlaying = false;

	}

}
