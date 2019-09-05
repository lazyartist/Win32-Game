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
	virtual void UpdateControllerImpl();
	virtual void UpdateRenderImpl();
	virtual void ReleaseImpl();

	void LoadSettings(const char * filePath);
	void SaveSettings(const char *szCurDir, const char * filePath);
	void LoadUnit(const char *filePath);
	void SaveUnit(const char *filePath);
	void AddUnitFilePath(CFilePath &cFilePath);
	void DeleteUnitFilePath(int index);

	vector<CFilePath> vecUnitFilePaths;
	vector<CUnit*> vecUnitsOnCanvas;
	CUnit cUnit;
	CController cController;
};