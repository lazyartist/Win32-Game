#pragma once
#include "lib.h"
#include "CGameFrame.h"

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

	CUnit cUnit;
};