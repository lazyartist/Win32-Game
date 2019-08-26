#pragma once
#include "lib.h"
#include "CGameFrame.h"

class CGameFrame_UnitStatePattern :
	public CGameFrame
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

	UnitStatePattern UnitStatePattern;
	vector<UnitState> UnitStates;
	UINT UnitStateIndex;
	UINT SelectedUnitStateIndex;
	bool IsPlaying;

private :
	fXY _xyClientCenter;
	Unit _unit;
	DWORD _unitIdleTime = 0;
};

