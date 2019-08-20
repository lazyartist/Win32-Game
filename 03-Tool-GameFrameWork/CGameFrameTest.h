#pragma once
#include "CGameFrame.h"

class CGameFrameTest : 
	public CGameFrame
{
public:
	CGameFrameTest();
	~CGameFrameTest();

	virtual void InitImpl();
	virtual void UpdateLogicImpl();
	virtual void UpdateRenderImpl();
	virtual void ReleaseImpl();
};

