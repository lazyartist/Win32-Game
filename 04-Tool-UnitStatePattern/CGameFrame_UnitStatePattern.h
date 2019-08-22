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
	void AddUnitState(UnitState unitState);
	void LoadUnitStatePatternFile(const char * filePath);
	void SaveUnitStatePatternFile(const char *filePath);

	vector<UnitState> UnitStates;
	UINT UnitStateIndex;
	bool IsPlaying;

private :
	fXY _xyClientCenter;
	Unit _unit;
};

