#include "stdafx.h"
#include "lib.h"
#include "CStageCreator.h"
#include "CGameFramework.cpp"

CStageCreator::CStageCreator() {
}
CStageCreator::~CStageCreator() {
}
void CStageCreator::InitImpl() {
	//pUnit.Init(_hdcMem);
}
void CStageCreator::UpdateLogicImpl() {
	for (size_t i = 0; i < cUnits.size(); i++) {
		CUnit &cUnit = cUnits[i];
		cUnit.Update(_fDeltaTime);
	}
}
void CStageCreator::UpdateControllerImpl() {
	if (pUnit != nullptr) {
		cController.Update(_fDeltaTime, pUnit);
	}
}
void CStageCreator::UpdateRenderImpl() {
	for (size_t i = 0; i < cUnits.size(); i++) {
		CUnit &cUnit = cUnits[i];
		cUnit.cActionList.RenderActions(_hdcMem);
		cUnit.Render(_hdcMem);
	}
}
void CStageCreator::ReleaseImpl() {
}
bool CStageCreator::LoadSettings(const char *szCurDir, const char *filePath) {
	char fullPath[MAX_PATH] = {};
	sprintf_s(fullPath, "%s\\%s", szCurDir, filePath);
	FILE *file = nullptr;
	file = _fsopen(fullPath, "rt", _SH_DENYNO);
	if (file != nullptr) {
		Func::FGetStr(cStageFilePath.szFileTitle, Const::szMax_Path, file);
		Func::FGetStr(cStageFilePath.szFilePath, Const::szMax_Path, file);
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
		char szBuffer[Const::szMax_Path] = {};
		//bg bmp path 
		Func::FGetStr(cBgiFilePath.szFileTitle, Const::szMax_Path, file);
		Func::FGetStr(cBgiFilePath.szFilePath, Const::szMax_Path, file);
		//controll unit iIndex
		int iControllUnitIndex = Func::FGetInt(szBuffer, Const::szMax_Path, file);
		//units
		int iCount = Func::FGetInt(szBuffer, Const::szMax_Path, file);
		int iUnitCount = cUnits.size();
		for (size_t i = 0; i < iCount; i++) {
			if (i >= iUnitCount) {
				cUnits.push_back(CUnit());
			}
			CUnit &cUnit = cUnits[i];
			cUnit.Init(_hdcMem);
			Func::FGetStr(cUnit.cFilePath.szFileTitle, Const::szMax_Path, file);
			Func::FGetStr(cUnit.cFilePath.szFilePath, Const::szMax_Path, file);
			cUnit.LoadUnit(&cUnit.cFilePath);
			cUnit.sStartXY.x = Func::FGetFloat(szBuffer, Const::szMax_Path, file);
			cUnit.sStartXY.y = Func::FGetFloat(szBuffer, Const::szMax_Path, file);
			cUnit.Reset();
		}
		SetControlUnit(iControllUnitIndex);
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
		//controll unit iIndex
		fprintf_s(file, "%i\n", iControlUnitIndex);
		//unit count
		int itemCount = cUnits.size();
		fprintf_s(file, "%i\n", itemCount);
		//units
		for (size_t i = 0; i < itemCount; i++) {
			CUnit &cUnit = cUnits[i];
			fprintf_s(file, "%s\n", cUnit.cFilePath.szFileTitle);
			fprintf_s(file, "%s\n", cUnit.cFilePath.szFilePath);
			fprintf_s(file, "%f\n", cUnit.sXY.x);
			fprintf_s(file, "%f\n", cUnit.sXY.y);
		}
		fclose(file);
	}
}
void CStageCreator::AddUnit(CFilePath &cFilePath) {
	FILE *file = nullptr;
	file = _fsopen(cFilePath.szFilePath, "rt", _SH_DENYNO);
	if (file != nullptr) {
		CUnit cUnit;
		cUnit.Init(_hdcMem);
		cUnit.LoadUnit(&cFilePath);
		cUnits.push_back(cUnit);
		fclose(file);
	}
}
void CStageCreator::RemoveUnit(int index) {
	auto iter = cUnits.begin();
	cUnits.erase(iter + index);
}
void CStageCreator::Reset() {
	for (size_t i = 0; i < cUnits.size(); i++) {
		CUnit &cUnit = cUnits[i];
		cUnit.Reset();
		//��Ʈ�������� �׼��������� �������� �ʰ� ����
		if (i == iControlUnitIndex) {
			cUnit.cActionList.Clear();
		}
	}
}

void CStageCreator::SetControlUnit(int iIndex) {
	if (iIndex < 0 || iIndex >= cUnits.size()) {
		return;
	}
	iControlUnitIndex = iIndex;
	pUnit = &cUnits[iIndex];
	SXY sXY = pUnit->sXY;
	pUnit->Reset();
	pUnit->cActionList.Clear();
	pUnit->sXY = sXY; // ��Ʈ���������� ������ ���� ��ġ�� �ֵ��� xy���� �����ߴٰ� �ٽ� �־���
}

CUnit * CStageCreator::GetControlUnit() {
	return nullptr;
}
