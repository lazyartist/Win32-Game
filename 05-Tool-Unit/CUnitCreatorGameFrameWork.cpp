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
	if (cUnit.bInitialized) {
		// collision update

		// game logic update
		cUnit.Update(_fDeltaTime);
	}
}
void CUnitCreatorGameFrameWork::UpdateControllerImpl() {
	cController.Update(_fDeltaTime, &cUnit);
}
void CUnitCreatorGameFrameWork::UpdateRenderImpl() {
	if (cUnit.bInitialized) {
		cUnit.cUnitStateAction.RenderUnitState(_hdcMem);
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
			char itemLine[Const::szMax_ItemLine] = {};
			char *token;
			char *nextToken;

			fgets(itemLine, Const::szMax_ItemLine, file);
			RemoveCarriageReturn(itemLine);
			nextToken = itemLine;
			token = strtok_s(nullptr, "\t", &nextToken);
			strcpy_s(cFilePath.szFileTitle, MAX_PATH, token);
			token = strtok_s(nullptr, "\t", &nextToken);
			strcpy_s(cFilePath.szFilePath, MAX_PATH, token);

			AddUnitFilePath(cFilePath);
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
			fprintf_s(file, "%s\t%s\n", CFilePath.szFileTitle, CFilePath.szFilePath);
		}
		fclose(file);
	}
}
void CUnitCreatorGameFrameWork::LoadUnit(const char *filePath) {
	FILE *file = nullptr;
	file = _fsopen(filePath, "rt", _SH_DENYNO);
	if (file != nullptr) {
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
		fgets(cUnit.cUnitStateAction.szFilePath, MAX_PATH, file);
		RemoveCarriageReturn(cUnit.cUnitStateAction.szFilePath);
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
	}

	// load bitmap
	cUnit.LoadUnitBitmap(cUnit.szBitmapPath);
	// load .usp
	cUnit.cUnitStatePattern.LoadUnitStatePatternFile(cUnit.cUnitStateAction.szFilePath);
	cUnit.cUnitStateAction = cUnit.cUnitStatePattern;
	// load .ani
	for (size_t i = 0; i < EUnitStateType::Count; i++) {
		cUnit.LoadAniFile((EUnitStateType)i, cUnit.arAniInfos[i].FilePath);
	}

	cUnit.bInitialized = true;
}
void CUnitCreatorGameFrameWork::SaveUnit(const char *filePath) {
	FILE *file = nullptr;
	file = _fsopen(filePath, "wt", _SH_DENYNO);
	if (file != nullptr) {
		// name
		fprintf_s(file, "%s\n", cUnit.szName);
		// fMagnification
		fprintf_s(file, "%f\n", cUnit.fMagnification);
		// fSpeedPerSeconds
		fprintf_s(file, "%f\n", cUnit.fSpeedPerSeconds);
		// bitmap path
		fprintf_s(file, "%s\n", cUnit.szBitmapPath);
		// pattern file
		fprintf_s(file, "%s\n", cUnit.cUnitStateAction.szFilePath);
		// ani files
		int itemCount = EUnitStateType::Count;
		fprintf_s(file, "%d\n", itemCount);
		for (size_t i = 0; i < itemCount; i++) {
			fprintf_s(file, "%s\t%s\n", cUnit.arAniInfos[i].FilePath, cUnit.arAniInfos[i].FileTitle);
		}
		fclose(file);
	};
}
void CUnitCreatorGameFrameWork::AddUnitFilePath(CFilePath & cFilePath) {
	vecUnitFilePaths.push_back(cFilePath);
}
void CUnitCreatorGameFrameWork::DeleteUnitFilePath(int index) {
	auto iter = vecUnitFilePaths.begin();
	vecUnitFilePaths.erase(iter + index);
}
