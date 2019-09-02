#include "stdafx.h"
#include "lib.h"
#include "CUnitCreatorGameFrameWork.h"
#include "CGameFrame.cpp"

CUnitCreatorGameFrameWork::CUnitCreatorGameFrameWork() {
}
CUnitCreatorGameFrameWork::~CUnitCreatorGameFrameWork() {
}
void CUnitCreatorGameFrameWork::InitImpl() {
	cUnit.Init(_hdcMem);
}
void CUnitCreatorGameFrameWork::UpdateLogicImpl() {
	if (bInitializedUnit) {
		// collision update

		// game logic update
		cUnit.Update(_fDeltaTime);
		// controll update
		cController.Update(_fDeltaTime, &cUnit);
	}
}
void CUnitCreatorGameFrameWork::UpdateRenderImpl() {
	if (bInitializedUnit) {
		cUnit.cUnitStatePattern.RenderUnitState(_hdcMem);
		cUnit.Render(_hdcMem);
	}
}
void CUnitCreatorGameFrameWork::ReleaseImpl() {
}
void CUnitCreatorGameFrameWork::LoadSettings(const char *filePath) {
	char curDir[Const::szMax_Path] = {};//프로그램의 작업 경로
	GetCurrentDirectory(Const::szMax_Path, curDir);
	char fullPath[Const::szMax_Path] = {};//exe 파일 경로
	sprintf_s(fullPath, "%s\\%s", curDir, filePath);

	FILE *file = nullptr;
	file = _fsopen(fullPath, "rt", _SH_DENYNO);
	if (file != nullptr) {
		char szItemCount[Const::szMax_ItemCount];
		fgets(szItemCount, Const::szMax_ItemCount, file);
		int iItemCount = atoi(szItemCount);
		for (size_t i = 0; i < iItemCount; i++) {
			CFilePath cFilePath;
			fgets(cFilePath.szFilePath, Const::szMax_Path, file);
			RemoveCarriageReturn(cFilePath.szFilePath);
			vecUnitFilePaths.push_back(cFilePath);

			//LVITEM item = {};
			//item.mask = LVIF_TEXT;
			//item.state;
			//item.stateMask;
			//item.iItem = i;
			//item.iSubItem = 0; // 아이템을 처음 추가하므로 0번째 서브아이템을 선택한다.
			//char itemText[MAX_PATH];
			//strcpy_s(itemText, MAX_PATH, g_szUnitStateTypeAsString[i]);
			//item.pszText = itemText;
			//ListView_InsertItem(g_hUnitList, &item); // 아이템 추가0
			//ListView_SetItemText(g_hUnitList, i, 1, g_cUnitCreator.cUnit.arAniInfos[i].FileTitle); // 아이템 추가0
		}
		fclose(file);
	}
}
void CUnitCreatorGameFrameWork::SaveSettings(const char *szCurDir, const char *filePath) {
	char fullPath[MAX_PATH] = {};
	sprintf_s(fullPath, "%s\\%s", szCurDir, filePath);

	FILE *file = nullptr;
	file = _fsopen(fullPath, "wt", _SH_DENYNO);

	if (file != nullptr) {
		int itemCount = vecUnitFilePaths.size();
		char szItemCount[Const::szMax_ItemCount] = {};
		_itoa_s(itemCount, szItemCount, Const::szMax_ItemCount, 10);
		fputs(szItemCount, file);
		fputs("\n", file);
		for (size_t i = 0; i < itemCount; i++) {
			char szItemText[Const::szMax_Path] = {};
			CFilePath &CFilePath = vecUnitFilePaths[i];
			fputs(CFilePath.szFilePath, file);
			fputs("\n", file);
		}
		fclose(file);
	}
}
void CUnitCreatorGameFrameWork::LoadUnit(const char *filePath) {
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
	for (size_t i = 0; i < itemCount; i++) {
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

	// load bitmap
	cUnit.LoadUnitBitmap(cUnit.szBitmapPath);
	// load .usp
	cUnit.cUnitStatePattern.LoadUnitStatePatternFile(cUnit.cUnitStatePattern.szFilePath);
	// load .ani
	for (size_t i = 0; i < EUnitStateType::Count; i++) {
		cUnit.LoadAniFile((EUnitStateType)i, cUnit.arAniInfos[i].FilePath);
	}

	bInitializedUnit = true;
}
void CUnitCreatorGameFrameWork::SaveUnit(const char *filePath) {
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
	for (size_t i = 0; i < itemCount; i++) {
		fprintf_s(file, "%s\t%s\n", cUnit.arAniInfos[i].FilePath, cUnit.arAniInfos[i].FileTitle);
	}

	fclose(file);
}

void CUnitCreatorGameFrameWork::BindControllerAndUnit() {
	//cController.Init(&cUnit);
}
