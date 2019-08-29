#include "stdafx.h"
#include "lib.h"
#include "CGameFrame_Unit.h"
#include "CGameFrame.cpp"


CUnitCreatorGameFrameWork::CUnitCreatorGameFrameWork()
{
}


CUnitCreatorGameFrameWork::~CUnitCreatorGameFrameWork()
{
}

void CUnitCreatorGameFrameWork::InitImpl()
{
	//dlog("CUnitCreatorGameFrameWork");
	//cUnit.SetName("hihi");
	cUnit.Init(_hdcMem);
}

void CUnitCreatorGameFrameWork::UpdateLogicImpl()
{
	//dlog("CUnitCreatorGameFrameWork::UpdateLogicImpl");

	cUnit.Update(_fDeltaTime);
}

void CUnitCreatorGameFrameWork::UpdateRenderImpl()
{
	cUnit.cUnitStatePattern.RenderUnitState(_hdcMem);
	cUnit.Render(_hdcMem);
}

void CUnitCreatorGameFrameWork::ReleaseImpl()
{
}

void CUnitCreatorGameFrameWork::LoadUnit(const char *filePath)
{
	FILE *file = nullptr;
	file = _fsopen(filePath, "rt", _SH_DENYNO);

	if (file == nullptr) return;

	// name
	fgets(cUnit.szName, szMax_UnitName, file);
	RemoveCarriageReturn(cUnit.szName);

	// fMagnification
	char szFloat[FLT_MAX_10_EXP];
	fgets(szFloat, FLT_MAX_10_EXP, file);
	RemoveCarriageReturn(szFloat);
	cUnit.fMagnification = atof(szFloat);

	// fSpeedPerSeconds
	fgets(szFloat, FLT_MAX_10_EXP, file);
	RemoveCarriageReturn(szFloat);
	cUnit.fSpeedPerSeconds = atof(szFloat);
	
	// bitmap file path
	fgets(cUnit.szBitmapPath, MAX_PATH, file);
	RemoveCarriageReturn(cUnit.szBitmapPath);

	// pattern file path
	fgets(cUnit.cUnitStatePattern.szFilePath, MAX_PATH, file);
	RemoveCarriageReturn(cUnit.cUnitStatePattern.szFilePath);

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

		SAniInfo aniInfo;

		token = strtok_s(nullptr, "\t", &nextToken);
		strcpy_s(aniInfo.FilePath, MAX_PATH, token);

		token = strtok_s(nullptr, "\t", &nextToken);
		strcpy_s(aniInfo.FileTitle, MAX_PATH, token);

		cUnit.arAniInfos[i] = aniInfo;
	}


	fclose(file);
}

void CUnitCreatorGameFrameWork::SaveUnit(const char *filePath)
{
	FILE *file = nullptr;
	file = _fsopen(filePath, "wt", _SH_DENYNO);

	if (file == nullptr) return;

	// name
	fprintf_s(file, "%s\n", cUnit.szName);

	// fMagnification
	fprintf_s(file, "%f\n", cUnit.fMagnification);

	// fSpeedPerSeconds
	fprintf_s(file, "%f\n", cUnit.fSpeedPerSeconds);

	// bitmap path
	fprintf_s(file, "%s\n", cUnit.szBitmapPath);

	// pattern file
	fprintf_s(file, "%s\n", cUnit.cUnitStatePattern.szFilePath);

	// ani files
	int itemCount = EUnitStateType::Count;
	fprintf_s(file, "%d\n", itemCount);
	for (size_t i = 0; i < itemCount; i++)
	{
		fprintf_s(file, "%s\t%s\n", cUnit.arAniInfos[i].FilePath, cUnit.arAniInfos[i].FileTitle);
	}

	fclose(file);
}

