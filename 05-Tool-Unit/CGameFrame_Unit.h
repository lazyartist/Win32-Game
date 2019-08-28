#pragma once
#include "lib.h"
#include "CGameFrame.h"

class CGameFrame_Unit :
	public CGameFrame
{
public:
	CGameFrame_Unit();
	virtual ~CGameFrame_Unit();

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
	Unit Unit;

	Unit *Unit2;

	Unit *Unit3;




};

