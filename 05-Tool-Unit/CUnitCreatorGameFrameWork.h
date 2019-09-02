#pragma once
#include <vector>
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

	void LoadSettings(const char * filePath);
	void SaveSettings(const char *szCurDir, const char * filePath);
	void LoadUnit(const char *filePath);
	void SaveUnit(const char *filePath);
	void BindControllerAndUnit();

	vector<CFilePath> vecUnitFilePaths;
	bool bInitializedUnit = false;
	CUnit cUnit;
	CController cController;
};