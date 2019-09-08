#pragma once
#include <vector>
#include "lib.h"
#include "CGameFramework.h"
#include "CController.h"

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
	void AddUnit(CFilePath & cFilePath);
	void RemoveUnit(int index);
	void Reset();
	void SetControlUnit(int index);
	CUnit* GetControlUnit();
	void SetStartXY();
	void LoadBgi(CFilePath &cFilePath);

	CFilePath cStageFilePath;
	CFilePath cBgiFilePath;
	vector<CUnit> cUnits; // todo : 포인터로 교체
	int iControlUnitIndex;
	CUnit *pUnit = nullptr;
	CController cController;
	HDC _hdcBgi;
};

