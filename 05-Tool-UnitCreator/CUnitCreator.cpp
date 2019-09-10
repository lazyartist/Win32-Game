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
		//unit type
		fprintf_s(file, "%i\n", cUnit.eUnitType);
		// ani files
		int itemCount = EActionType::EActionType_Count;
		fprintf_s(file, "%d\n", itemCount);
		for (size_t i = 0; i < itemCount; i++) {
			fprintf_s(file, "%s\n", cUnit.arAniInfos[i].FileTitle);
			fprintf_s(file, "%s\n", cUnit.arAniInfos[i].FilePath);
		}
		fclose(file);
	};
}
void CUnitCreator::SetUnitType(int index, EUnitType eUnitType) {
	cUnit.eUnitType = eUnitType;
}
void CUnitCreator::AddUnitFilePath(CFilePath & cFilePath) {
	vecUnitFilePaths.push_back(cFilePath);
}
void CUnitCreator::DeleteUnitFilePath(int index) {
	auto iter = vecUnitFilePaths.begin();
	vecUnitFilePaths.erase(iter + index);
}
