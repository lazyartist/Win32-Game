#pragma once
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
};

