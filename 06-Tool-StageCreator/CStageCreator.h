#pragma once
#include "CGameFramework.h";
#include "CController.h";

class CStageCreator :
	public CGameFramework{
public:
	CStageCreator();
	virtual ~CStageCreator();

	virtual void InitImpl();
	virtual void UpdateLogicImpl();
	virtual void UpdateControllerImpl();
	virtual void UpdateRenderImpl();
	virtual void ReleaseImpl();

	bool LoadSettings(const char *szCurDir, const char * filePath);
	void SaveSettings(const char *szCurDir, const char *filePath);
	void LoadStage(const CFilePath &cFilePath);
	void SaveStage(const CFilePath &cFilePath);
	void AddUnitFilePath(CFilePath &cFilePath);
	void DeleteUnitFilePath(int index);

	CFilePath cStageFilePath;
	CFilePath cBgiFilePath;
	vector<CFilePath> cUnitFilePaths;
	vector<CUnit> cUnits;
	int iUnitIndex;
	CUnit *cUnit;
	CController cController;
};

