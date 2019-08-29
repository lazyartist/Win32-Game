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

	//vector<AniFilePath> AniFilePaths;
	//vector<char *> AniFilePaths;
	//vector<char *> AniFilePaths;
	//vector<char [MAX_PATH]> AniFilePaths;
	//char AniFilePath[MAX_PATH];
	CUnit cUnit;

	//cUnit *Unit2;

	//cUnit *Unit3;




};

