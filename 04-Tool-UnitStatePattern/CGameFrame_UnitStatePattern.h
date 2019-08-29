#pragma once
#include "lib.h"
#include "CGameFrame.h"

class CGameFrame_UnitStatePattern :
	public CGameFrameWorkBase
{
public:
	CGameFrame_UnitStatePattern();
	virtual ~CGameFrame_UnitStatePattern();

	virtual void InitImpl();
	virtual void UpdateLogicImpl();
	virtual void UpdateRenderImpl();
	virtual void ReleaseImpl();

	void DrawUnit();
	void DrawUnitState();
	void InitUnit();

	void PlayStop(bool isPlay);

	CUnitStatePattern UnitStatePattern;
	vector<CUnitState> UnitStates;
	UINT UnitStateIndex;
	UINT SelectedUnitStateIndex;
	bool IsPlaying;

private :
	SXY _xyClientCenter;
	CUnit _unit;
	DWORD _unitIdleTime = 0;
};

