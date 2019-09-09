#include "stdafx.h"
#include "lib.h"
#include "CUnitCreator.h"
#include "CGameFramework.cpp"

CUnitCreator::CUnitCreator() {
}
CUnitCreator::~CUnitCreator() {
}
void CUnitCreator::InitImpl() {
	cUnit.Init(_hdcMem);
}
void CUnitCreator::UpdateLogicImpl() {
	if (cUnit.bInitialized) {
		// collision update

		// game logic update
		cUnit.Update(_fDeltaTime);
	}
}
void CUnitCreator::UpdateControllerImpl() {
	cController.Update(_fDeltaTime, &cUnit);
}
void CUnitCreator::UpdateRenderImpl() {
	if (cUnit.bInitialized) {
		cUnit.cActionList.RenderActions(_hdcMem);
		cUnit.Render(_hdcMem);
	}
}
void CUnitCreator::ReleaseImpl() {
}
void CUnitCreator::LoadSettings(const char *filePath) {
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
void CUnitCreator::SaveSettings(const char *szCurDir, const char *filePath) {
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
void CUnitCreator::LoadUnit(const char *filePath) {
	FILE *file = nullptr;
	file = _fsopen(filePath, "rt", _SH_DENYNO);
	if (file != nullptr) {
		cUnit.Reset();
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
		fgets(cUnit.cActionList.szFilePath, MAX_PATH, file);
		RemoveCarriageReturn(cUnit.cActionList.szFilePath);
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
	cUnit.cActionListPattern.LoadActionPatternFile(cUnit.cActionList.szFilePath);
	cUnit.cActionList = cUnit.cActionListPattern;
	// load .ani
	for (size_t i = 0; i < EActionType::EActionType_Count; i++) {
		cUnit.LoadAniFile((EActionType)i, cUnit.arAniInfos[i].FilePath);
	}

	cUnit.bInitialized = true;
}
void CUnitCreator::SaveUnit(const char *filePath) {
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
		fprintf_s(file, "%s\n", cUnit.cActionList.szFilePath);
		// ani files
		int itemCount = EActionType::EActionType_Count;
		fprintf_s(file, "%d\n", itemCount);
		for (size_t i = 0; i < itemCount; i++) {
			fprintf_s(file, "%s\t%s\n", cUnit.arAniInfos[i].FilePath, cUnit.arAniInfos[i].FileTitle);
		}
		fclose(file);
	};
}
void CUnitCreator::AddUnitFilePath(CFilePath & cFilePath) {
	vecUnitFilePaths.push_back(cFilePath);
}
void CUnitCreator::DeleteUnitFilePath(int index) {
	auto iter = vecUnitFilePaths.begin();
	vecUnitFilePaths.erase(iter + index);
}
