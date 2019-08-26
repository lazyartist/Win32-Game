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

	//HDC hdcMem = CreateCompatibleDC(_hdcMem);
	////HBITMAP hOldBitmap = CreateCompatibleBitmap(hdcMem, 0, 0);
	//AniInfo aniFile = Unit.AniInfos[(int)Unit.CurUnitStateType];
	//(HBITMAP)SelectObject(hdcMem, aniFile.hBitmap);

	//TransparentBlt(_hdcMem, 0, 0, 100, 100, hdcMem, 0, 0, 100, 100, RGB(255, 0, 0));

	////SelectObject(hdcMem, hOldBitmap);


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
	
	// bitmap path
	fgets(Unit.BitmapPath, MAX_PATH, file);
	RemoveCarriageReturn(Unit.BitmapPath);

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
	// bitmap path
	fprintf_s(file, "%s\n", Unit.BitmapPath);

	// ani files
	int itemCount = UnitStateType::Count;
	fprintf_s(file, "%d\n", itemCount);
	for (size_t i = 0; i < itemCount; i++)
	{
		fprintf_s(file, "%s\t%s\n", Unit.AniInfos[i].FilePath, Unit.AniInfos[i].FileTitle);
	}

	/*


			char szItemCount[szMax_UnitStateCount] = {};
			sprintf_s<szMax_UnitStateCount>(szItemCount, "%d\n", itemCount);
			fputs(szItemCount, file);

			auto iter = UnitStates.begin();
			while (iter != UnitStates.end())
			{
				fprintf_s(file, "%d\t%f\t%f\t%d\n", iter->UnitStateType, iter->XY.x, iter->XY.y, iter->Time);

				++iter;
			}*/

	fclose(file);
}

