#pragma once
#include "CGameFrame.h"

class CGameFrameTest : 
	public CGameFrame
{
public:
	CGameFrameTest();
	~CGameFrameTest();

	virtual void InitImpl();
	virtual void UpdateImpl();
	virtual void RenderImpl();
	virtual void ReleaseImpl();
};

