#pragma once
#include "lib.h"
#include "CGameFrame.h"

class CActionPatternGameFrameWork :
	public CGameFrameWorkBase
{
public:
	CActionPatternGameFrameWork();
	virtual ~CActionPatternGameFrameWork();

	virtual void InitImpl();
	virtual void UpdateLogicImpl();
	virtual void UpdateRenderImpl();
	virtual void ReleaseImpl();

	void DrawUnit();
	void DrawAction();
	void InitUnit();

	void PlayStop(bool isPlay);

	CActions cActions;
	vector<CAction> vecActions;
	UINT iSelectedActionIndex;
	bool IsPlaying;

private :
	SXY _xyClientCenter;
	CUnit _unit;
	DWORD _unitIdleTime = 0;
};

