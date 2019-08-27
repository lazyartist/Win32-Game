#include "stdafx.h"
#include "lib.h"
#include "CGameFrame_Unit.h"
#include "CGameFrame.cpp"


CGameFrame_Unit::CGameFrame_Unit()
{
}


CGameFrame_Unit::~CGameFrame_Unit()
{
}

void CGameFrame_Unit::InitImpl()
{
	//dlog("CGameFrame_Unit");
	//Unit.SetName("hihi");
	Unit.Init(_hdcMem);
}

void CGameFrame_Unit::UpdateLogicImpl()
{
	//dlog("CGameFrame_Unit::UpdateLogicImpl");

	Unit.Update();
}

void CGameFrame_Unit::UpdateRenderImpl()
{
	Unit.UnitStatePattern.RenderUnitState(_hdcMem);
	Unit.Render(_hdcMem);
}

void CGameFrame_Unit::ReleaseImpl()
{
}

void CGameFrame_Unit::LoadUnit(const char *filePath)
{
	FILE *file = nullptr;
	file = _fsopen(filePath, "rt", _SH_DENYNO);

	if (file == nullptr) return;

	// name
	fgets(Unit.Name, szMax_UnitName, file);
	RemoveCarriageReturn(Unit.Name);

	// Magnification
	char szMagnification[FLT_MAX_10_EXP];
	fgets(szMagnification, FLT_MAX_10_EXP, file);
	RemoveCarriageReturn(szMagnification);
	Unit.Magnification = atof(szMagnification);
	
	// bitmap file path
	fgets(Unit.BitmapPath, MAX_PATH, file);
	RemoveCarriageReturn(Unit.BitmapPath);

	// pattern file path
	fgets(Unit.UnitStatePattern.FilePath, MAX_PATH, file);
	RemoveCarriageReturn(Unit.UnitStatePattern.FilePath);

	// ani files
	char szItemCount[szMax_PosLine] = {};
	fgets(szItemCount, szMax_PosLine, file);
	RemoveCarriageReturn(szItemCount);

	int itemCount = atoi(szItemCount);
	char itemLine[szMax_PosLine] = {};
	for (size_t i = 0; i < itemCount; i++)
	{
		// ===== List에 아이템 추가 =====
		memset(itemLine, 0, szMax_PosLine);

		fgets(itemLine, szMax_PosLine, file);
		RemoveCarriageReturn(itemLine);

		char *token;
		char *nextToken;

		nextToken = itemLine;

		AniInfo aniInfo;

		token = strtok_s(nullptr, "\t", &nextToken);
		strcpy_s(aniInfo.FilePath, MAX_PATH, token);

		token = strtok_s(nullptr, "\t", &nextToken);
		strcpy_s(aniInfo.FileTitle, MAX_PATH, token);

		Unit.AniInfos[i] = aniInfo;
	}


	fclose(file);
}

void CGameFrame_Unit::SaveUnit(const char *filePath)
{
	FILE *file = nullptr;
	file = _fsopen(filePath, "wt", _SH_DENYNO);

	if (file == nullptr) return;

	// name
	fprintf_s(file, "%s\n", Unit.Name);

	// Magnification
	fprintf_s(file, "%f\n", Unit.Magnification);

	// bitmap path
	fprintf_s(file, "%s\n", Unit.BitmapPath);

	// pattern file
	fprintf_s(file, "%s\n", Unit.UnitStatePattern.FilePath);

	// ani files
	int itemCount = UnitStateType::Count;
	fprintf_s(file, "%d\n", itemCount);
	for (size_t i = 0; i < itemCount; i++)
	{
		fprintf_s(file, "%s\t%s\n", Unit.AniInfos[i].FilePath, Unit.AniInfos[i].FileTitle);
	}

	fclose(file);
}

