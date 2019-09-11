#include "stdafx.h"
#include "lib.h"
#include "Physics.h"

const char *Const::szActionTypesAsString[EActionType::EActionType_Count] = { "EActionType_Idle" , "EActionType_MoveTo", "EActionType_Shoot" };
const char *Const::szUnitTypesAsString[EUnitType::EUnitType_Count] = { "EUnitType_Unit" , "EUnitType_Ball", "EUnitType_Goal" };
const char *Const::szControlTypesAsString[EControlType::EControlType_Count] = { "EControlType_Player" , "EControlType_AI", "EControlType_Pattern" };
const char *Const::szStageSettingFileName = "stageCreator.settings";

CFilePath::CFilePath() {
	szFilePath[0] = 0;
	szFileTitle[0] = 0;
}
void CFilePath::Clear() {
	szFilePath[0] = 0;
	szFileTitle[0] = 0;
}

CUnit::CUnit() {
	for (size_t i = 0; i < EActionType::EActionType_Count; i++) {
		arAniInfos[i].FilePath[0] = 0;
		arAniInfos[i].FileTitle[0] = 0;
	}
}
CUnit::~CUnit() {}
CUnit& CUnit::operator=(const CUnit& r) {
	//포인터 또는 가변길이로 인해 객체의 데이터가 메모리상 일직선이 아닐 경우 일괄적으로 memcpy를 사용하면 안되고 개별 복사해줘야한다.
	//memcpy(this, &r, sizeof(*this));

	memcpy(&this->cFilePath, &r.cFilePath, sizeof(this->cFilePath));

	for (size_t i = 0; i < EActionType::EActionType_Count; i++) {
		this->arAniInfos[i] = r.arAniInfos[i];
		this->arAniInfos[i].SpriteInfos = r.arAniInfos[i].SpriteInfos;
		//memcpy(&this->arAniInfos[i], &r.arAniInfos[i], sizeof(this->arAniInfos[i]));
	}
	this->cActionList = r.cActionList;
	//memcpy(&this->cActionList, &r.cActionList, sizeof(this->cActionList));
	this->cActionList.cActions = r.cActionList.cActions;

	this->cActionListPattern = r.cActionListPattern;
	//memcpy(&this->cActionListPattern, &r.cActionListPattern, sizeof(this->cActionListPattern));
	this->cActionListPattern.cActions = r.cActionListPattern.cActions;

	this->_cCurSpriteInfo = r._cCurSpriteInfo;
	//memcpy(&this->_cCurSpriteInfo, &r._cCurSpriteInfo, sizeof(this->_cCurSpriteInfo));
	this->_cCurSpriteInfo.sLocalCollisions = r._cCurSpriteInfo.sLocalCollisions;

	return (*this);
}

void CUnit::Init(HDC hdc) {
	if (hBitmapDC) {
		DeleteDC(hBitmapDC);
	}
	hBitmapDC = CreateCompatibleDC(hdc);
}
void CUnit::Update(float fDeltaTime) {
	CAction *curAction = &cActionList.GetCurAction();
	if (curAction->IsDone()) {
		NextAction();
		curAction = &cActionList.GetCurAction();
	}

	const vector<CSpriteInfo> * spriteInfos = &arAniInfos[(int)curAction->eActionType].SpriteInfos;
	if (spriteInfos->size() != 0) {
		_cCurSpriteInfo = (*spriteInfos)[_iAniIndex]; // todo : to pointer
	}


	bool bEndAni = false;//애니메이션이 끝났는지 여부, 애니가 끝까지 재생되고 _iAniIndex가 0으로 갱신되면 true가 된다.

	// update ani
	// 경과 시간을 보고 _iAniIndex를 갱신
	time_t time = GetTickCount();
	if (time - _iAniTime >= _cCurSpriteInfo.iTime) {
		_iAniTime = time;
		size_t size = spriteInfos->size();
		if (size != 0) {
			++_iAniIndex;
			if (_iAniIndex >= spriteInfos->size()) {
				_iAniIndex = 0;
				bEndAni = true;
			}
			_cCurSpriteInfo = (*spriteInfos)[_iAniIndex];
		}
		else {
			_iAniIndex = 0;
		}
	}
	//control unit
	if (this->eControlType == EControlType::EControlType_Player) {
		CUnit *otherUnit = Physics::hitTest(*this);
		if (otherUnit) {
			this->cSubUnit = otherUnit;
		}
	}
	//sub unit
	if (cSubUnit != nullptr) {
		if (curAction->eActionType == EActionType::EActionType_MoveTo) {
			CAction cAction;
			cAction.sXY = this->sXY;
			cAction.sXY.Add(100, 0);
			CActionFactory::MoveTo(*cSubUnit, &cAction, fDeltaTime, false); // 이동 목표지점은 방향이 아니라 절대 좌표이다.
			cSubUnit->AddAction(cAction);
		}
	}
	// 액션 상태 갱신1
	if (bEndAni && !curAction->bRepeat) {
		// ani가 끝났고 한번만 재생하는 액션이라면 액션 갱신하고 스프라이트 정보 다시 읽기
		curAction->Done();
		curAction = &cActionList.GetCurAction();
		spriteInfos = &arAniInfos[(int)curAction->eActionType].SpriteInfos;
		_cCurSpriteInfo = (*spriteInfos)[_iAniIndex];
	}
	// 액션 상태 갱신2
	if (curAction->eActionType == EActionType::EActionType_Idle) {
		if (_iWaitTimeOnPosition == 0) {
			_iWaitTimeOnPosition = GetTickCount();
			//return;
		}
		else {
			if (curAction->iTime != INT_MAX && GetTickCount() - _iWaitTimeOnPosition >= curAction->iTime) {
				// end wait
				_iWaitTimeOnPosition = 0;
				curAction->Done();
			}
			else {
				// wait
				//return;
			}
		}
	}
	else if (curAction->eActionType == EActionType::EActionType_MoveTo) {
		// 다음 지점까지의 거리
		SXY distanceXY = curAction->sXY - sXY;
		float distance = distanceXY.distance();
		float speed = fSpeedPerSeconds * fDeltaTime;

		if (distance < speed) {
			speed = distance;
		}

		_fDirectionRadian = atan2(distanceXY.y, distanceXY.x);
		_v2NormalDirection = { cos(_fDirectionRadian) , sin(_fDirectionRadian) }; // 각도로 x, y 좌표를 얻었으므로 정규화 되어있다.
		_fDirectionWithCosRight = _v2NormalDirection.x * kV2Right.x + _v2NormalDirection.y * kV2Right.y;

		float speedX = speed * _v2NormalDirection.x;
		float speedY = speed * _v2NormalDirection.y;
		sXY.x += speedX;
		sXY.y += speedY;

		if (sameXY(curAction->sXY, sXY)) {
			curAction->Done();
		}
	}
	else if (curAction->eActionType == EActionType::EActionType_Shoot) {
		if (this->eControlType == EControlType::EControlType_Player && cSubUnit != nullptr) {
			CAction cAction;
			cAction.sXY = { 700, this->sXY.y };
			CActionFactory::MoveTo(*cSubUnit, &cAction, fDeltaTime, false); // 이동 목표지점은 방향이 아니라 절대 좌표이다.
			cSubUnit->AddAction(cAction);
			cSubUnit = nullptr;
		}
	}
}
void CUnit::Render(HDC hdc) {
	//if (!bPlaying) return;
	// test
	//fWH g_whBottomWndSize = { 800, 600 };
	// 화면 정중앙 좌표
	//SXY center = { g_whBottomWndSize.w / 2, g_whBottomWndSize.h / 2 };

	// 선택된 스프라이트의 크기
	fWH spriteSize = { _cCurSpriteInfo.sRect.right - _cCurSpriteInfo.sRect.left, _cCurSpriteInfo.sRect.bottom - _cCurSpriteInfo.sRect.top };
	if (spriteSize.w >= sBitmapHeader.bmWidth) {
		spriteSize.w = sBitmapHeader.bmWidth - 1;
	}
	if (spriteSize.h >= sBitmapHeader.bmHeight) {
		spriteSize.h = sBitmapHeader.bmHeight - 1;
	}
	UINT w = _cCurSpriteInfo.sRect.right - _cCurSpriteInfo.sRect.left;
	UINT h = _cCurSpriteInfo.sRect.bottom - _cCurSpriteInfo.sRect.top;
	// TransparentBlt를 사용하려면 프로젝트 설정에서 msimg32.lib;를 추가종속성에 받드시 추가해야한다.
	TransparentBlt(hdc,
		sXY.x - _cCurSpriteInfo.sPivot.x * fMagnification,
		sXY.y - _cCurSpriteInfo.sPivot.y * fMagnification,
		spriteSize.w * fMagnification,
		spriteSize.h * fMagnification,
		hBitmapDC,
		_cCurSpriteInfo.sRect.left,
		_cCurSpriteInfo.sRect.top,
		spriteSize.w,
		spriteSize.h,

		RGB(255, 0, 0));
	//g_BitmapViewInfo.TransparentColor);

	//collision
	//SetROP2(hdc, R2_BLACK      );//항상 검정색이다.
	//SetROP2(hdc, R2_NOTMERGEPEN);
	//SetROP2(hdc, R2_MASKNOTPEN );
	//SetROP2(hdc, R2_NOTCOPYPEN );//새 그림을 반전시켜 그린다.
	//SetROP2(hdc, R2_MASKPENNOT );
	//SetROP2(hdc, R2_NOT        );//원래의 그림을 반전시킨다.
	//SetROP2(hdc, R2_XORPEN     );//XOR연산으로 겹치는 부분만 반전시킨다.
	//SetROP2(hdc, R2_NOTMASKPEN );//원래의 그림을 반전시키고 AND연산으로 겹치는 부분만 그린다.
	//SetROP2(hdc, R2_MASKPEN    );//AND연산으로 겹치는 부분만 그린다.
	//SetROP2(hdc, R2_NOTXORPEN  );
	//SetROP2(hdc, R2_NOP        );//아무런 그리기도 하지 않는다.
	//SetROP2(hdc, R2_MERGENOTPEN);//새 그림을 반전시키고 OR연산으로 합친다.
	//SetROP2(hdc, R2_COPYPEN    );//원래의 그림을 덮어버리고 새 그림을 그린다.
	//SetROP2(hdc, R2_MERGEPENNOT);
	//SetROP2(hdc, R2_MERGEPEN   );//OR연산으로 두 그림을 합친다.
	//SetROP2(hdc, R2_WHITE      );//항상 흰색이다.
	//SetROP2(hdc, R2_LAST       );
	if (Physics::bEnable) {
		//HBRUSH hBrush = CreateSolidBrush(RGB(128, 128, 128));
		HBRUSH hBrush = CreateSolidBrush(RGB(255, 255, 255));
		//HBRUSH hBrush = CreateSolidBrush(RGB(0, 0, 0));
		HBRUSH hOldBrush = (HBRUSH)SelectObject(hdc, hBrush);
		for (size_t i = 0; i < _cCurSpriteInfo.iCollisionCount; i++) {
			CUnit *ohterUnit = Physics::hitTest(*this);
			if (ohterUnit) {
				SetROP2(hdc, R2_NOTMASKPEN);//원래의 그림을 반전시키고 AND연산으로 겹치는 부분만 그린다.
			}
			else {
				SetROP2(hdc, R2_MASKPEN);//AND연산으로 겹치는 부분만 그린다.
			}
			RECT collision = GetCollision();
			Rectangle(hdc, collision.left, collision.top, collision.right, collision.bottom);
			//Rectangle(hdc,
			//	sXY.x - _cCurSpriteInfo.sPivot.x * fMagnification,
			//	sXY.y - _cCurSpriteInfo.sPivot.y * fMagnification,
			//	sXY.x - _cCurSpriteInfo.sPivot.x * fMagnification + col.right * fMagnification,
			//	sXY.y - _cCurSpriteInfo.sPivot.y * fMagnification + col.bottom * fMagnification);
		}
		SetROP2(hdc, R2_COPYPEN);//default
		hBrush = (HBRUSH)SelectObject(hdc, hOldBrush);
		DeleteObject(hBrush);
	}

	// test : draw direction
	float _directionLength = 50;
	SXY _directionXY = { _directionLength * _v2NormalDirection.x, _directionLength * _v2NormalDirection.y };
	MoveToEx(hdc, sXY.x, sXY.y, nullptr);
	LineTo(hdc, sXY.x + _directionXY.x, sXY.y + _directionXY.y);
	char szText[FLT_MAX_10_EXP] = {};
	//// test : text radian
	//sprintf_s(szText, FLT_MAX_10_EXP, "%f", _fDirectionRadian);
	//TextOut(hdc, 0, 0, szText, strlen(szText));
	//// test : text direction
	//sprintf_s(szText, FLT_MAX_10_EXP, "%f", _fDirectionWithCosRight);
	//TextOut(hdc, 0, 100, szText, strlen(szText));
	//if (_fDirectionWithCosRight > 0) {
	//	sprintf_s(szText, FLT_MAX_10_EXP, "%s", "right");
	//}
	//else {
	//	sprintf_s(szText, FLT_MAX_10_EXP, "%s", "left");
	//}
	//TextOut(hdc, 0, 200, szText, strlen(szText));
	// test : text _iAniIndex
	CAction curAction = cActionList.GetCurAction();
	vector<CSpriteInfo> &spriteInfos = arAniInfos[(int)curAction.eActionType].SpriteInfos;
	sprintf_s(szText, FLT_MAX_10_EXP, "%d/%d", _iAniIndex, spriteInfos.size());
	TextOut(hdc, sXY.x, sXY.y + 2, szText, strlen(szText));
	//RECT rectText = { sXY.x, sXY.y , sXY.x + 100, sXY.y + 100 };
	//DrawText(hdc, szText, strlen(szText), &rectText, DT_LEFT);
}
void CUnit::Reset() {
	_iAniIndex = 0;
	_iAniTime = GetTickCount();
	// 초기 위치 설정
	cActionList.Init();
	cActionListPattern.Init();
	if (eControlType == EControlType::EControlType_Pattern) {
		CopyAction();
	}
	sXY = sStartXY;
	//sXY = cAction.sXY;
	CAction cAction = cActionList.GetCurAction();
	const vector<CSpriteInfo> * spriteInfos = &arAniInfos[(int)cAction.eActionType].SpriteInfos;
	if (spriteInfos->size() != 0) {
		_cCurSpriteInfo = (*spriteInfos)[_iAniIndex];
	}
}
void CUnit::ClearAni() {
	_iAniIndex = 0;
	//_iAniTime = 0;
	_iAniTime = GetTickCount();
}void CUnit::ClearAction() {
	cActionList.Clear();
	ClearAni();
	//cActionListPattern.Clear();
}
void CUnit::Clear() {
	ClearAni();
	cActionList.Clear();
	cActionListPattern.Clear();
	szName[0] = 0;
	SXY sXY = { 0, 0 };
	WH sWH = { 0, 0 };
	float fSpeedPerSeconds = 10.0;
	float fMagnification = 1.0;
	//EActionType eCurActionType;
	szBitmapPath[0] = 0;
	for (size_t i = 0; i < static_cast<EActionType>(EActionType::EActionType_Count); i++) {
		arAniInfos[i].FilePath[0] = 0;
		arAniInfos[i].FileTitle[0] = 0;
		arAniInfos[i].SpriteInfos.clear();
	}
	ClearUnitBitmap();

	bInitialized = false;
}
void CUnit::LoadUnit(CFilePath *cFilePath) {
	FILE *file = nullptr;
	file = _fsopen(cFilePath->szFilePath, "rt", _SH_DENYNO);
	if (file != nullptr) {
		char itemLine[Const::szMax_ItemLine] = {};
		Reset();
		this->cFilePath = *cFilePath;
		// name
		Func::FGetStr(szName, szMax_UnitName, file);
		// fMagnification
		fMagnification = Func::FGetFloat(itemLine, Const::szMax_ItemLine, file);
		// fSpeedPerSeconds
		fSpeedPerSeconds = Func::FGetFloat(itemLine, Const::szMax_ItemLine, file);
		// bitmap file path
		Func::FGetStr(szBitmapPath, Const::szMax_Path, file);
		// pattern file path
		Func::FGetStr(cActionList.szFilePath, Const::szMax_Path, file);
		// unit type
		int iUnitType = Func::FGetInt(itemLine, Const::szMax_ItemLine, file);
		eUnitType = (EUnitType)iUnitType;
		// ani files
		int itemCount = Func::FGetInt(itemLine, Const::szMax_ItemLine, file);
		for (size_t i = 0; i < itemCount; i++) {
			SAniInfo aniInfo;
			Func::FGetStr(aniInfo.FileTitle, Const::szMax_Path, file);
			Func::FGetStr(aniInfo.FilePath, Const::szMax_Path, file);
			arAniInfos[i] = aniInfo;
		}
		fclose(file);
	}
	// load bitmap
	LoadUnitBitmap(szBitmapPath);
	// load .usp
	cActionListPattern.LoadActionPatternFile(cActionList.szFilePath);
	if (eControlType == EControlType::EControlType_Pattern) {
		CopyAction();
	}
	// load .ani
	for (size_t i = 0; i < EActionType::EActionType_Count; i++) {
		LoadAniFile((EActionType)i, arAniInfos[i].FilePath);
	}

	bInitialized = true;
}
void CUnit::SetName(const char *name) {
	strcpy_s(szName, szMax_UnitName, name);
}
void CUnit::LoadUnitBitmap(const char *filePath) {
	strcpy_s(szBitmapPath, MAX_PATH, filePath);
	HBITMAP hBitmap = (HBITMAP)LoadImage(nullptr, filePath, IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
	GetObject(hBitmap, sizeof(BITMAP), &sBitmapHeader);
	SelectObject(hBitmapDC, hBitmap); // HBITMAP은 HDC에 적용되면 다시 사용할 수 없기 때문에 재사용을 위해 HDC에 넣어둔다.
	DeleteObject(hBitmap);
}
void CUnit::ClearUnitBitmap() {
	HBITMAP hBitmap = CreateCompatibleBitmap(hBitmapDC, 100, 100);
	SelectObject(hBitmapDC, hBitmap); // HBITMAP은 HDC에 적용되면 다시 사용할 수 없기 때문에 재사용을 위해 HDC에 넣어둔다.
	DeleteObject(hBitmap);
}
void CUnit::LoadAniFile(EActionType eActionType, const char *filePath) {
	FILE *file = nullptr;
	file = _fsopen(filePath, "rt", _SH_DENYNO);

	if (file == nullptr) return;

	SAniInfo &aniFile = arAniInfos[(int)eActionType];
	strcpy_s(aniFile.FilePath, MAX_PATH, filePath);
	strcpy_s(aniFile.FileTitle, MAX_PATH, GetFileNameByFullPath(filePath));
	aniFile.SpriteInfos.clear();

	char temp[MAX_PATH];
	fgets(temp, MAX_PATH, file); // bitmap path 사용하지 않으므로 임시 변수에 가져온다.(한 줄 건너뛰는 효과)

	char szItemCount[szMax_SpriteCount] = {};
	fgets(szItemCount, szMax_SpriteCount, file);

	// item count
	int itemCount = atoi(szItemCount);

	aniFile.SpriteInfos.reserve(itemCount);

	// ===== List에 아이템 추가 =====
	char itemLine[szMax_PosLine] = {};
	char *token;
	char *nextToken;
	for (size_t i = 0; i < itemCount; i++) {
		memset(itemLine, 0, szMax_PosLine);
		fgets(itemLine, szMax_PosLine, file);
		// \n은 줄바꿈을 지정하는 문자이므로 순수 문자만 얻기 위해 제거한다.
		itemLine[strcspn(itemLine, "\n")] = 0; // strcspn()으로 "\n"의 위치를 찾고 그 위치에 0을 넣어준다.

		if (strnlen_s(itemLine, szMax_PosLine) == 0) continue;

		CSpriteInfo spriteInfo;
		nextToken = itemLine;
		token = strtok_s(nullptr, "\t", &nextToken);
		spriteInfo.iTime = atoi(token);
		// rect, pivot
		int coordinates[nMax_SpriteCoordinateCount] = {  };
		for (size_t j = 0; j < nMax_SpriteCoordinateCount; j++) {
			token = strtok_s(nullptr, "\t", &nextToken);
			coordinates[j] = atoi(token);
		}
		spriteInfo.SetCoordinates(coordinates, sizeof(INT) * nMax_SpriteCoordinateCount);
		// collision
		token = strtok_s(nullptr, "\t", &nextToken);
		INT collisionCount = atoi(token);
		spriteInfo.ResetCollisionCount();
		for (size_t j = 0; j < collisionCount; j++) {
			int collisionPos[nMax_RectPos];
			for (size_t k = 0; k < nMax_RectPos; k++) {
				token = strtok_s(nullptr, "\t", &nextToken);
				collisionPos[k] = atoi(token);
			}

			spriteInfo.AddCollision(collisionPos, nMax_RectPos);
		}
		aniFile.SpriteInfos.push_back(spriteInfo);
	}
	fclose(file);
}
void CUnit::NextAction() {
	cActionList.NextAction();
	_iAniIndex = 0;
	_iWaitTimeOnPosition = GetTickCount();

	if (eControlType == EControlType::EControlType_Pattern && cActionList.cActions.size() == 0) {
		// 패턴으로 조작하는 유닛은 패턴을 다시 넣어줌
		CopyAction();
	}
}
void CUnit::CopyAction() {
	cActionList = cActionListPattern;
}
RECT CUnit::GetCollision() const {
	RECT collision = _cCurSpriteInfo.sLocalCollisions[0];
	RECT globalCollision = {
		sXY.x - _cCurSpriteInfo.sPivot.x * fMagnification,
		sXY.y - _cCurSpriteInfo.sPivot.y * fMagnification,
		sXY.x - _cCurSpriteInfo.sPivot.x * fMagnification + collision.right * fMagnification,
		sXY.y - _cCurSpriteInfo.sPivot.y * fMagnification + collision.bottom * fMagnification };
	return globalCollision;
}
void CUnit::AddAction(const CAction &cAction) {
	CAction cCurAction = cActionList.GetCurAction();
	if (cAction.eActionType == EActionType::EActionType_MoveTo) {
		if (cCurAction.eActionType != EActionType::EActionType_MoveTo) {
			ClearAni(); //현재 이동중이라면 애니메이션을 초기화하지 않고 연속 재생한다.
		}
		cActionList.Clear();
		cActionList.AddAction(cAction);
	}
	else if (cAction.eActionType == EActionType::EActionType_Shoot) {
		ClearAni();
		cActionList.Clear();
		cActionList.AddAction(cAction);
	}

	//CAction cNoneAction;
	//cNoneAction.eActionType == EActionType::EActionType_None;
	//cActionList.AddAction(cNoneAction);
}
void CActionFactory::MoveTo(const CUnit &cUnit, CAction *cAction, float fDeltaTime, bool bDirection) {
	cAction->eActionType = EActionType::EActionType_MoveTo;
	//방향이라면 유닛의 스피드로 이동목표지점을 계산한다.
	if (bDirection) {
		// 이동 거리로 x, y값을 계산(이렇게 하지 않으면 x, y를 동시에 움직일 경우 더 많은 거리를 이동한다)
		float _fDirectionRadian = atan2(cAction->sXY.y, cAction->sXY.x);
		cAction->sXY = { cos(_fDirectionRadian) , sin(_fDirectionRadian) }; // 각도로 x, y 좌표를 얻었으므로 정규화 되어있다.
		cAction->sXY = { cAction->sXY.x * cUnit.fSpeedPerSeconds * fDeltaTime, cAction->sXY.y * cUnit.fSpeedPerSeconds * fDeltaTime };
		//cAction->sXY = { cAction->sXY.x * cUnit.fSpeedPerSeconds * fDeltaTime * Const::fSpeedPerFrameMagnification(), cAction->sXY.y * cUnit.fSpeedPerSeconds * fDeltaTime * Const::fSpeedPerFrameMagnification() };
		cAction->sXY.Add(cUnit.sXY.x, cUnit.sXY.y);
	}
	cAction->bCancelable = true;
	cAction->bRepeat = true;
}
void CActionFactory::Shoot(const CUnit &cUnit, CAction *cAction) {
	cAction->eActionType = EActionType::EActionType_Shoot;
	cAction->bCancelable = false;
	cAction->bRepeat = false;
}