#pragma once
#include "lib.h"
#include "CGameFramework.h"

class CActionPattern :
	public CGameFramework
{
public:
	CActionPattern();
	virtual ~CActionPattern();

	virtual void InitImpl();
	virtual void UpdateLogicImpl();
	virtual void UpdateRenderImpl();
	virtual void ReleaseImpl();

	void DrawUnit();
	void DrawAction();
	void InitUnit();

	void PlayStop(bool isPlay);

	CActionList cActionList;
	UINT iSelectedActionIndex;
	bool IsPlaying;

private :
	SXY _xyClientCenter;
	CUnit _unit;
	DWORD _unitIdleTime = 0;
};

