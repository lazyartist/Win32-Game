#pragma once
#include "CGameFramework.h"

class CGameFrameworkTest : 
	public CGameFramework
{
public:
	CGameFrameworkTest();
	~CGameFrameworkTest();

	virtual void InitImpl();
	virtual void UpdateLogicImpl();
	virtual void UpdateRenderImpl();
	virtual void ReleaseImpl();
};

