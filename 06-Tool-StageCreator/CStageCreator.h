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
	void SetControlType(int index, EControlType eControlType);
	CUnit* GetControlUnit();
	void SetStartXY(int index);
	void LoadBgi(CFilePath &cFilePath);

	CFilePath cStageFilePath;
	CFilePath cBgiFilePath;
	HDC hdcBgi;
	BITMAP sBgiHeader;
	float fBgiMagnification = 1.0;
	vector<CUnit> cUnits; // todo : �����ͷ� ��ü
	int iControlUnitIndex;
	CUnit *pControlUnit = nullptr;
	CController cController;
	EStageEndType eStageEndType = EStageEndType::EStageEndType_None;
};

