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
		Func::FGets(cStageFilePath.szFilePath, Const::szMax_Path, file);
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
		int iUnitCount = cUnits.size();
		for (size_t i = 0; i < iCount; i++) {
			if (i >= iUnitCount) {
				cUnits.push_back(CUnit());
			}
			CUnit &cUnit = cUnits[i];
			cUnit.Init(_hdcMem);
			Func::FGets(cUnit.cFilePath.szFileTitle, Const::szMax_Path, file);
			Func::FGets(cUnit.cFilePath.szFilePath, Const::szMax_Path, file);
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
			CUnit &cUnit = cUnits[i];
			fprintf_s(file, "%s\n", cUnit.cFilePath.szFileTitle);
			fprintf_s(file, "%s\n", cUnit.cFilePath.szFilePath);
			fprintf_s(file, "%i\n", cUnit.sXY.x);
			fprintf_s(file, "%i\n", cUnit.sXY.y);
		}
		fclose(file);
	}
}
void CStageCreator::AddUnit(CFilePath &cFilePath) {
	FILE *file = nullptr;
	file = _fsopen(cFilePath.szFilePath, "rt", _SH_DENYNO);
	if (file != nullptr) {
		CUnit cUnit;
		cUnit.cFilePath = cFilePath;
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
		// load bitmap
		cUnit.LoadUnitBitmap(cUnit.szBitmapPath);
		// load .usp
		cUnit.cActionListPattern.LoadActionPatternFile(cUnit.cActionList.szFilePath);
		cUnit.cActionList = cUnit.cActionListPattern;
		// load .ani
		for (size_t i = 0; i < EActionType::Count; i++) {
			cUnit.LoadAniFile((EActionType)i, cUnit.arAniInfos[i].FilePath);
		}
		cUnit.bInitialized = true;
		cUnits.push_back(cUnit);

		fclose(file);
	}
}
void CStageCreator::RemoveUnit(int index) {
	auto iter = cUnits.begin();
	cUnits.erase(iter + index);
}