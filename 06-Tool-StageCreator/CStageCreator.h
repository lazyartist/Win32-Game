#pragma once
#include "CGameFramework.h";

class CStageCreator :
	public CGameFramework{
public:
	CStageCreator();
	virtual ~CStageCreator();
	// implement derived class
	virtual void InitImpl();
	virtual void UpdateLogicImpl();
	virtual void UpdateControllerImpl();
	virtual void UpdateRenderImpl();
	virtual void ReleaseImpl();
};

