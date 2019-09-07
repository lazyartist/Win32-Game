#include "stdafx.h"
#include "CStageCreator.h"
#include "CGameFramework.cpp"

CStageCreator::CStageCreator() {
}
CStageCreator::~CStageCreator() {
}
void CStageCreator::InitImpl() {
	//cUnit.Init(_hdcMem);
}
void CStageCreator::UpdateLogicImpl() {
	//cUnit.Update(_fDeltaTime);
}
void CStageCreator::UpdateControllerImpl() {
	//cController.Update(_fDeltaTime, &cUnit);
}
void CStageCreator::UpdateRenderImpl() {
	//if (cUnit.bInitialized) {
	//	cUnit.cActionList.RenderActions(_hdcMem);
	//	cUnit.Render(_hdcMem);
	//}
}
void CStageCreator::ReleaseImpl() {
}
bool CStageCreator::LoadSettings(const char *szCurDir, const char *filePath) {
	char fullPath[MAX_PATH] = {};
	sprintf_s(fullPath, "%s\\%s", szCurDir, filePath);
	FILE *file = nullptr;
	file = _fsopen(fullPath, "rt", _SH_DENYNO);
	if (file != nullptr) {
		Func::FGets(cStageFilePath.szFileTitle, Const::szMax_Path, file);
		Func::FGets(cStageFilePath.szFileTitle, Const::szMax_Path, file);
		fclose(file);
		return true;
	}
	return false;
}
void CStageCreator::SaveSettings(const char *szCurDir, const char *filePath) {
	char fullPath[MAX_PATH] = {};
	sprintf_s(fullPath, "%s\\%s", szCurDir, filePath);
	FILE *file = nullptr;
	file = _fsopen(fullPath, "wt", _SH_DENYNO);
	if (file != nullptr) {
		//.stage
		fprintf_s(file, "%s\n", cStageFilePath.szFileTitle);
		fprintf_s(file, "%s\n", cStageFilePath.szFilePath);
		fclose(file);
	}
}
void CStageCreator::LoadStage(const CFilePath &cFilePath) {
	FILE *file = nullptr;
	file = _fsopen(cFilePath.szFilePath, "rt", _SH_DENYNO);
	if (file != nullptr) {
		cStageFilePath = cFilePath;
		char szLine[Const::szMax_Path] = {};
		char *token;
		char *nextToken;
		//bg bmp path 
		Func::FGets(cBgiFilePath.szFileTitle, Const::szMax_Path, file);
		Func::FGets(cBgiFilePath.szFilePath, Const::szMax_Path, file);
		//controll unit index
		Func::FGets(szLine, Const::szMax_Path, file);
		iUnitIndex = atoi(szLine);
		//unit count
		Func::FGets(szLine, Const::szMax_Path, file);
		int iCount = atoi(szLine);
		//units
		int iFilePathCount = cUnitFilePaths.size();
		int iUnitCount = cUnits.size();
		for (size_t i = 0; i < iCount; i++) {
			if (i >= iFilePathCount) {
				cUnitFilePaths.push_back(CFilePath());
			}
			if (i >= iUnitCount) {
				cUnits.push_back(CUnit());
			}
			CFilePath &cUnitFilePath = cUnitFilePaths[i];
			CUnit &cUnit = cUnits[i];

			Func::FGets(cUnitFilePath.szFileTitle, Const::szMax_Path, file);
			Func::FGets(cUnitFilePath.szFilePath, Const::szMax_Path, file);
			Func::FGets(szLine, Const::szMax_Path, file);
			cUnit.sXY.x = atoi(szLine);
			Func::FGets(szLine, Const::szMax_Path, file);
			cUnit.sXY.y = atoi(szLine);
		}
		fclose(file);
	}
}
void CStageCreator::SaveStage(const CFilePath &cFilePath) {
	FILE *file = nullptr;
	file = _fsopen(cFilePath.szFilePath, "wt", _SH_DENYNO);
	if (file != nullptr) {
		cStageFilePath = cFilePath;
		//bg bmp path 
		fprintf_s(file, "%s\n", cBgiFilePath.szFileTitle);
		fprintf_s(file, "%s\n", cBgiFilePath.szFilePath);
		//controll unit index
		fprintf_s(file, "%i\n", iUnitIndex);
		//unit count
		int itemCount = cUnits.size();
		fprintf_s(file, "%i\n", itemCount);
		//units
		for (size_t i = 0; i < itemCount; i++) {
			CFilePath &cFilePath = cUnitFilePaths[i];
			CUnit &cUnit = cUnits[i];
			//filetitle filepath x y
			//char szItemText[Const::szMax_Path] = {};
			CFilePath &CFilePath = cUnitFilePaths[i];
			fprintf_s(file, "%s\n", cFilePath.szFileTitle);
			fprintf_s(file, "%s\n", cFilePath.szFilePath);
			fprintf_s(file, "%i\n", cUnit.sXY.x);
			fprintf_s(file, "%i\n", cUnit.sXY.y);
		}
		fclose(file);
	}
}
void CStageCreator::AddUnitFilePath(CFilePath & cFilePath) {
	cUnitFilePaths.push_back(cFilePath);
}
void CStageCreator::DeleteUnitFilePath(int index) {
	auto iter = cUnitFilePaths.begin();
	cUnitFilePaths.erase(iter + index);
}