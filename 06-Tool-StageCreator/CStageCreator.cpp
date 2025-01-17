#include "stdafx.h"
#include "lib.h"
#include "CStageCreator.h"
#include "CGameFramework.cpp"
#include "Physics.h"

CStageCreator::CStageCreator() {
}
CStageCreator::~CStageCreator() {
}
void CStageCreator::InitImpl() {
	hdcBgi = CreateCompatibleDC(_hdcMem);
	Physics::bEnable = true;
	Physics::cUnits = &cUnits;
}
void CStageCreator::UpdateLogicImpl() {

	CUnit *cBallUnit = nullptr;
	CUnit *cGoalUnit = nullptr;
	for (size_t i = 0; i < cUnits.size(); i++) {
		CUnit *cUnit = &cUnits[i];
		cUnit->Update(_fDeltaTime);
		if (cBallUnit == nullptr && cUnit->eUnitType == EUnitType::EUnitType_Ball) {
			cBallUnit = cUnit;
		}
		if (cGoalUnit == nullptr && cUnit->eUnitType == EUnitType::EUnitType_Goal) {
			cGoalUnit = cUnit;
		}
	}

	if (eStageEndType == EStageEndType::EStageEndType_None) {
		RECT intersectRect;
		//win
		if (cBallUnit && cGoalUnit && IntersectRect(&intersectRect, &cBallUnit->GetCollision(), &cGoalUnit->GetCollision()) != 0) {
			eStageEndType = EStageEndType::EStageEndType_Win;
			for (size_t i = 0; i < cUnits.size(); i++) {
				CUnit *cUnit = &cUnits[i];
				cUnit->Update(_fDeltaTime);
				if (pControlUnit == cUnit) {
					CAction cAction;
					CActionFactory::Win(*cUnit, &cAction);
					cUnit->AddAction(cAction);
				}
				else if (cUnit->eUnitType == EUnitType::EUnitType_Unit) {
					CAction cAction;
					CActionFactory::Lose(*cUnit, &cAction);
					cUnit->AddAction(cAction);
				}
				else if (cUnit->eUnitType == EUnitType::EUnitType_Goal) {
					cUnit->ClearAction();
				}
				else if (cUnit->eUnitType == EUnitType::EUnitType_Ball) {
					cUnit->ClearAction();
				}
			}
		}
	}
}
void CStageCreator::UpdateControllerImpl() {
	if (pControlUnit != nullptr) {
		cController.Update(_fDeltaTime, pControlUnit);
	}
}
void CStageCreator::UpdateRenderImpl() {
	//bgi
	TransparentBlt(_hdcMem,
		0, 0,
		sBgiHeader.bmWidth * fBgiMagnification, sBgiHeader.bmHeight * fBgiMagnification,
		hdcBgi,
		0, 0,
		sBgiHeader.bmWidth, sBgiHeader.bmHeight,
		RGB(255, 0, 0));

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
		//bgi path
		Func::FGetStr(cBgiFilePath.szFileTitle, Const::szMax_Path, file);
		Func::FGetStr(cBgiFilePath.szFilePath, Const::szMax_Path, file);
		LoadBgi(cBgiFilePath);
		//bgi magnification
		fBgiMagnification = Func::FGetFloat(szBuffer, Const::szMax_Path, file);
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
			cUnit.eControlType = static_cast<EControlType>(Func::FGetInt(szBuffer, Const::szMax_Path, file));
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
		//bgi path
		fprintf_s(file, "%s\n", cBgiFilePath.szFileTitle);
		fprintf_s(file, "%s\n", cBgiFilePath.szFilePath);
		//bgi magnification
		fprintf_s(file, "%f\n", fBgiMagnification);
		//controll unit iIndex
		fprintf_s(file, "%i\n", iControlUnitIndex);
		//unit count
		int itemCount = cUnits.size();
		fprintf_s(file, "%i\n", itemCount);
		//units
		for (size_t i = 0; i < itemCount; i++) {
			CUnit &cUnit = cUnits[i];
			fprintf_s(file, "%i\n", cUnit.eControlType);
			fprintf_s(file, "%s\n", cUnit.cFilePath.szFileTitle);
			fprintf_s(file, "%s\n", cUnit.cFilePath.szFilePath);
			fprintf_s(file, "%f\n", cUnit.sStartXY.x);
			fprintf_s(file, "%f\n", cUnit.sStartXY.y);
		}
		fclose(file);
	}
}
void CStageCreator::AddUnit(CFilePath &cFilePath) {
	FILE *file = nullptr;
	file = _fsopen(cFilePath.szFilePath, "rt", _SH_DENYNO);
	if (file != nullptr) {
		{
			CUnit cUnit;
			cUnit.Init(_hdcMem);
			cUnit.LoadUnit(&cFilePath);
			cUnits.push_back(cUnit);
			fclose(file);
		}
		// vector가 확장되며 cControlUnit가 댕글링 포인터가 될 가능성이 있으므로 다시 설정해준다.
		UpdateControlUnit();
	}
}
void CStageCreator::RemoveUnit(int index) {
	auto iter = cUnits.begin();
	cUnits.erase(iter + index);
	UpdateControlUnit();
}
void CStageCreator::UpdateControlUnit() {
	for (size_t i = 0; i < cUnits.size(); i++) {
		CUnit &cUnit = cUnits[i];
		if (cUnit.eControlType == EControlType::EControlType_Player) {
			SetControlUnit(i);
		}
	}
}
void CStageCreator::Reset() {
	eStageEndType = EStageEndType::EStageEndType_None;
	for (size_t i = 0; i < cUnits.size(); i++) {
		CUnit &cUnit = cUnits[i];
		cUnit.Reset();
		//컨트롤유닛은 액션패턴으로 움직이지 않게 제거
		if (i == iControlUnitIndex) {
			cUnit.cActionList.Clear();
		}
	}
}
void CStageCreator::SetControlUnit(int index) {
	if (index < 0 || index >= cUnits.size()) {
		return;
	}
	if (pControlUnit) {
		pControlUnit->eControlType = EControlType::EControlType_AI;
		pControlUnit->cSubUnit = nullptr;
	}
	iControlUnitIndex = index;
	pControlUnit = &cUnits[index];
	SXY sXY = pControlUnit->sXY;
	pControlUnit->Reset();
	pControlUnit->cActionList.Clear();
	pControlUnit->sXY = sXY; // 컨트롤유닛으로 지정시 현재 위치에 있도록 xy값을 저장했다가 다시 넣어줌
	pControlUnit->eControlType = EControlType::EControlType_Player;
}
void CStageCreator::SetControlType(int index, EControlType eControlType) {
	if (index < 0 || index >= cUnits.size()) {
		return;
	}
	CUnit &cUnit = cUnits[index];
	cUnit.eControlType = eControlType;

	if (cUnit.eControlType == EControlType::EControlType_Player) {
		SetControlUnit(index);
	}
}
CUnit * CStageCreator::GetControlUnit() {
	return pControlUnit;
}
void CStageCreator::SetStartXY(int index) {
	if (index < 0 || cUnits.size() <= index) {
		return;
	}
	CUnit &cUnit = cUnits[index];
	cUnit.sStartXY = cUnit.sXY;
}
void CStageCreator::LoadBgi(CFilePath &cFilePath) {
	//strcpy_s(szBitmapPath, MAX_PATH, filePath);
	cBgiFilePath = cFilePath;
	HBITMAP hBitmap = (HBITMAP)LoadImage(nullptr, cFilePath.szFilePath, IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
	SelectObject(hdcBgi, hBitmap); // HBITMAP은 HDC에 적용되면 다시 사용할 수 없기 때문에 재사용을 위해 HDC에 넣어둔다.
	GetObject(hBitmap, sizeof(BITMAP), &sBgiHeader);//bitmap 정보 읽기
	DeleteObject(hBitmap);
}