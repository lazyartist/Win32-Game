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

void CGameFrame_Unit::LoadUnit(char *filePath)
{
}

void CGameFrame_Unit::SaveUnit(char *filePath)
{
	FILE *file = nullptr;
	file = _fsopen(filePath, "wt", _SH_DENYNO);

	if (file == nullptr) return;

	fprintf_s(file, "%s\n", Unit.Name);



	// item count
	int itemCount = UnitStateType::Count;
	//int itemCount = Unit.AniInfos.size();
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

