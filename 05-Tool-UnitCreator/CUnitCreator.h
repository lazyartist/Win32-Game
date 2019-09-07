#pragma once
#include <vector>
#include "lib.h"
#include "CGameFramework.h"
#include "CController.h"

class CUnitCreator :
	public CGameFramework
{
public:
	CUnitCreator();
	virtual ~CUnitCreator();

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