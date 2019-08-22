#include "stdafx.h"
#include "lib.h"
#include "CGameFrame_UnitStatePattern.h"
#include "CGameFrame.cpp"
#include "04common.h"
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

		if (sameXY(curUnitState.xy, _unit.xy)) {
			if (curUnitState.UnitStateType == UnitStateType::Idle) {
				if (_unitIdleTime == 0) {
					_unitIdleTime = GetTickCount();
					return;
				}
				else {
					if (GetTickCount() - _unitIdleTime >= curUnitState.msTime) {
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

	Ellipse(_hdcMem, _unit.xy.x - _unit.wh.w / 2, _unit.xy.y - _unit.wh.h / 2,
		_unit.xy.x + _unit.wh.w / 2, _unit.xy.y + _unit.wh.w / 2);

	DeleteObject(SelectObject(_hdcMem, hOldPen));
}

void CGameFrame_UnitStatePattern::DrawUnitState()
{
	if (UnitStates.size() == 0) return;

	HPEN hLinePen = (HPEN)GetStockObject(BLACK_PEN);
	HPEN hIdlePen = CreatePen(PS_SOLID, 1, RGB(0, 255, 0));;
	HPEN hWalkPen = CreatePen(PS_SOLID, 1, RGB(0, 0, 255));;
	HPEN hSelectedPen = CreatePen(PS_SOLID, 1, RGB(255, 0, 0));

	HPEN hOldPen = (HPEN)SelectObject(_hdcMem, hWalkPen);

	MoveToEx(_hdcMem, UnitStates[0].xy.x, UnitStates[0].xy.y, nullptr);
	WH wh = { 10, 10 };

	for (size_t i = 0; i < UnitStates.size(); i++)
	{
		UnitState unitState = UnitStates[i];

		SelectObject(_hdcMem, hLinePen);
		LineTo(_hdcMem, unitState.xy.x, unitState.xy.y);

		if (i == SelectedUnitStateIndex) {
			SelectObject(_hdcMem, hSelectedPen);
		}
		else if (unitState.UnitStateType == UnitStateType::Idle) {
			SelectObject(_hdcMem, hIdlePen);
		}
		else if (unitState.UnitStateType == UnitStateType::Walk) {
			SelectObject(_hdcMem, hWalkPen);
		}

		Ellipse(_hdcMem, unitState.xy.x - wh.w / 2, unitState.xy.y - wh.h / 2,
			unitState.xy.x + wh.w / 2, unitState.xy.y + wh.w / 2);

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
	_unit.speedPerSeconds = 500;
	_unit.xy = _xyClientCenter;
	_unit.wh = { 30, 30 };

	UnitStateIndex = 0;
	SelectedUnitStateIndex = NoSelectedIndex;
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

void CGameFrame_UnitStatePattern::AddUnitState(UnitState unitState)
{
	UnitStates.push_back(unitState);
}

bool CGameFrame_UnitStatePattern::DeleteUnitState(UINT index)
{
	if (index >= UnitStates.size()) return false;

	auto iter = UnitStates.begin();
	UnitStates.erase(iter + index);

	return true;
}

bool CGameFrame_UnitStatePattern::DeleteAllUnitState()
{
	if (IsPlaying) return false;

	UnitStates.clear();

	return true;
}

bool CGameFrame_UnitStatePattern::UpUnitState(UINT index)
{
	if (index == 0 || index >= UnitStates.size()) return false;

	UnitState unitStateTemp = UnitStates[index - 1];
	UnitStates[index - 1] = UnitStates[index];
	UnitStates[index] = unitStateTemp;

	return true;
}

bool CGameFrame_UnitStatePattern::DownUnitState(UINT index)
{
	if (index < 0 || index >= UnitStates.size() - 1) return false;

	UnitState unitStateTemp = UnitStates[index + 1];
	UnitStates[index + 1] = UnitStates[index];
	UnitStates[index] = unitStateTemp;

	return true;
}

void CGameFrame_UnitStatePattern::LoadUnitStatePatternFile(const char *filePath) {
	FILE *file = nullptr;
	file = _fsopen(filePath, "rt", _SH_DENYNO);

	if (file == nullptr) return;

	char szItemCount[szMax_SpriteCount] = {};
	fgets(szItemCount, szMax_SpriteCount, file);

	// item count
	int itemCount = atoi(szItemCount);

	UnitStates.clear();
	UnitStates.reserve(itemCount);

	// ===== List에 아이템 추가 =====
	char itemLine[szMax_UnitStateLine] = {};
	char *token;
	char *nextToken;

	char itemText[szMax_XY] = {};
	for (size_t i = 0; i < itemCount; i++)
	{
		memset(itemLine, 0, szMax_UnitStateLine);
		fgets(itemLine, szMax_UnitStateLine, file);

		// \n은 줄바꿈을 지정하는 문자이므로 순수 문자만 얻기 위해 제거한다.
		itemLine[strcspn(itemLine, "\n")] = 0; // strcspn()으로 "\n"의 위치를 찾고 그 위치에 0을 넣어준다.

		if (strnlen_s(itemLine, szMax_PosLine) == 0) continue;

		UnitState unitState;
		{
			nextToken = itemLine;

			token = strtok_s(nullptr, "\t", &nextToken);
			unitState.UnitStateType = (UnitStateType)atoi(token);

			token = strtok_s(nullptr, "\t", &nextToken);
			unitState.xy.x = atof(token);
			token = strtok_s(nullptr, "\t", &nextToken);
			unitState.xy.y = atof(token);
			token = strtok_s(nullptr, "\t", &nextToken);
			unitState.msTime = atoi(token);
		}

		AddUnitState(unitState);
		//AddSpriteInfo(i, &unitState);
	}

	fclose(file);
}


void CGameFrame_UnitStatePattern::SaveUnitStatePatternFile(const char *filePath) {
	FILE *file = nullptr;
	file = _fsopen(filePath, "wt", _SH_DENYNO);

	if (file == nullptr) return;

	// item count
	int itemCount = UnitStates.size();
	char szItemCount[szMax_UnitStateCount] = {};
	sprintf_s<szMax_UnitStateCount>(szItemCount, "%d\n", itemCount);
	fputs(szItemCount, file);

	auto iter = UnitStates.begin();
	while (iter != UnitStates.end())
	{
		fprintf_s(file, "%d\t%f\t%f\t%d\n", iter->UnitStateType, iter->xy.x, iter->xy.y, iter->msTime);

		++iter;
	}

	fclose(file);
}