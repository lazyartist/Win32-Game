#pragma once
#include "lib.h"
#include "CGameFrame.h"
#include "CController.h"

class CUnitCreatorGameFrameWork :
	public CGameFrameWorkBase
{
public:
	CUnitCreatorGameFrameWork();
	virtual ~CUnitCreatorGameFrameWork();

	virtual void InitImpl();
	virtual void UpdateLogicImpl();
	virtual void UpdateRenderImpl();
	virtual void ReleaseImpl();

	void LoadUnit(const char *filePath);
	void SaveUnit(const char *filePath);
	void BindControllerAndUnit();

	CUnit cUnit;
	CController cController;
};