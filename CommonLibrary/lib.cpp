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
	//������ �Ǵ� �������̷� ���� ��ü�� �����Ͱ� �޸𸮻� �������� �ƴ� ��� �ϰ������� memcpy�� ����ϸ� �ȵǰ� ���� ����������Ѵ�.
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


	bool bEndAni = false;//�ִϸ��̼��� �������� ����, �ִϰ� ������ ����ǰ� _iAniIndex�� 0���� ���ŵǸ� true�� �ȴ�.

	// update ani
	// ��� �ð��� ���� _iAniIndex�� ����
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
			CActionFactory::MoveTo(*cSubUnit, &cAction, fDeltaTime, false); // �̵� ��ǥ������ ������ �ƴ϶� ���� ��ǥ�̴�.
			cSubUnit->AddAction(cAction);
		}
	}
	// �׼� ���� ����1
	if (bEndAni && !curAction->bRepeat) {
		// ani�� ������ �ѹ��� ����ϴ� �׼��̶�� �׼� �����ϰ� ��������Ʈ ���� �ٽ� �б�
		curAction->Done();
		curAction = &cActionList.GetCurAction();
		spriteInfos = &arAniInfos[(int)curAction->eActionType].SpriteInfos;
		_cCurSpriteInfo = (*spriteInfos)[_iAniIndex];
	}
	// �׼� ���� ����2
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
		// ���� ���������� �Ÿ�
		SXY distanceXY = curAction->sXY - sXY;
		float distance = distanceXY.distance();
		float speed = fSpeedPerSeconds * fDeltaTime;

		if (distance < speed) {
			speed = distance;
		}

		_fDirectionRadian = atan2(distanceXY.y, distanceXY.x);
		_v2NormalDirection = { cos(_fDirectionRadian) , sin(_fDirectionRadian) }; // ������ x, y ��ǥ�� ������Ƿ� ����ȭ �Ǿ��ִ�.
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
			CActionFactory::MoveTo(*cSubUnit, &cAction, fDeltaTime, false); // �̵� ��ǥ������ ������ �ƴ϶� ���� ��ǥ�̴�.
			cSubUnit->AddAction(cAction);
			cSubUnit = nullptr;
		}
	}
}
void CUnit::Render(HDC hdc) {
	//if (!bPlaying) return;
	// test
	//fWH g_whBottomWndSize = { 800, 600 };
	// ȭ�� ���߾� ��ǥ
	//SXY center = { g_whBottomWndSize.w / 2, g_whBottomWndSize.h / 2 };

	// ���õ� ��������Ʈ�� ũ��
	fWH spriteSize = { _cCurSpriteInfo.sRect.right - _cCurSpriteInfo.sRect.left, _cCurSpriteInfo.sRect.bottom - _cCurSpriteInfo.sRect.top };
	if (spriteSize.w >= sBitmapHeader.bmWidth) {
		spriteSize.w = sBitmapHeader.bmWidth - 1;
	}
	if (spriteSize.h >= sBitmapHeader.bmHeight) {
		spriteSize.h = sBitmapHeader.bmHeight - 1;
	}
	UINT w = _cCurSpriteInfo.sRect.right - _cCurSpriteInfo.sRect.left;
	UINT h = _cCurSpriteInfo.sRect.bottom - _cCurSpriteInfo.sRect.top;
	// TransparentBlt�� ����Ϸ��� ������Ʈ �������� msimg32.lib;�� �߰����Ӽ��� �޵�� �߰��ؾ��Ѵ�.
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
	//SetROP2(hdc, R2_BLACK      );//�׻� �������̴�.
	//SetROP2(hdc, R2_NOTMERGEPEN);
	//SetROP2(hdc, R2_MASKNOTPEN );
	//SetROP2(hdc, R2_NOTCOPYPEN );//�� �׸��� �������� �׸���.
	//SetROP2(hdc, R2_MASKPENNOT );
	//SetROP2(hdc, R2_NOT        );//������ �׸��� ������Ų��.
	//SetROP2(hdc, R2_XORPEN     );//XOR�������� ��ġ�� �κи� ������Ų��.
	//SetROP2(hdc, R2_NOTMASKPEN );//������ �׸��� ������Ű�� AND�������� ��ġ�� �κи� �׸���.
	//SetROP2(hdc, R2_MASKPEN    );//AND�������� ��ġ�� �κи� �׸���.
	//SetROP2(hdc, R2_NOTXORPEN  );
	//SetROP2(hdc, R2_NOP        );//�ƹ��� �׸��⵵ ���� �ʴ´�.
	//SetROP2(hdc, R2_MERGENOTPEN);//�� �׸��� ������Ű�� OR�������� ��ģ��.
	//SetROP2(hdc, R2_COPYPEN    );//������ �׸��� ��������� �� �׸��� �׸���.
	//SetROP2(hdc, R2_MERGEPENNOT);
	//SetROP2(hdc, R2_MERGEPEN   );//OR�������� �� �׸��� ��ģ��.
	//SetROP2(hdc, R2_WHITE      );//�׻� ����̴�.
	//SetROP2(hdc, R2_LAST       );
	if (Physics::bEnable) {
		//HBRUSH hBrush = CreateSolidBrush(RGB(128, 128, 128));
		HBRUSH hBrush = CreateSolidBrush(RGB(255, 255, 255));
		//HBRUSH hBrush = CreateSolidBrush(RGB(0, 0, 0));
		HBRUSH hOldBrush = (HBRUSH)SelectObject(hdc, hBrush);
		for (size_t i = 0; i < _cCurSpriteInfo.iCollisionCount; i++) {
			CUnit *ohterUnit = Physics::hitTest(*this);
			if (ohterUnit) {
				SetROP2(hdc, R2_NOTMASKPEN);//������ �׸��� ������Ű�� AND�������� ��ġ�� �κи� �׸���.
			}
			else {
				SetROP2(hdc, R2_MASKPEN);//AND�������� ��ġ�� �κи� �׸���.
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
	// �ʱ� ��ġ ����
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
	SelectObject(hBitmapDC, hBitmap); // HBITMAP�� HDC�� ����Ǹ� �ٽ� ����� �� ���� ������ ������ ���� HDC�� �־�д�.
	DeleteObject(hBitmap);
}
void CUnit::ClearUnitBitmap() {
	HBITMAP hBitmap = CreateCompatibleBitmap(hBitmapDC, 100, 100);
	SelectObject(hBitmapDC, hBitmap); // HBITMAP�� HDC�� ����Ǹ� �ٽ� ����� �� ���� ������ ������ ���� HDC�� �־�д�.
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
	fgets(temp, MAX_PATH, file); // bitmap path ������� �����Ƿ� �ӽ� ������ �����´�.(�� �� �ǳʶٴ� ȿ��)

	char szItemCount[szMax_SpriteCount] = {};
	fgets(szItemCount, szMax_SpriteCount, file);

	// item count
	int itemCount = atoi(szItemCount);

	aniFile.SpriteInfos.reserve(itemCount);

	// ===== List�� ������ �߰� =====
	char itemLine[szMax_PosLine] = {};
	char *token;
	char *nextToken;
	for (size_t i = 0; i < itemCount; i++) {
		memset(itemLine, 0, szMax_PosLine);
		fgets(itemLine, szMax_PosLine, file);
		// \n�� �ٹٲ��� �����ϴ� �����̹Ƿ� ���� ���ڸ� ��� ���� �����Ѵ�.
		itemLine[strcspn(itemLine, "\n")] = 0; // strcspn()���� "\n"�� ��ġ�� ã�� �� ��ġ�� 0�� �־��ش�.

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
		// �������� �����ϴ� ������ ������ �ٽ� �־���
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
			ClearAni(); //���� �̵����̶�� �ִϸ��̼��� �ʱ�ȭ���� �ʰ� ���� ����Ѵ�.
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
	//�����̶�� ������ ���ǵ�� �̵���ǥ������ ����Ѵ�.
	if (bDirection) {
		// �̵� �Ÿ��� x, y���� ���(�̷��� ���� ������ x, y�� ���ÿ� ������ ��� �� ���� �Ÿ��� �̵��Ѵ�)
		float _fDirectionRadian = atan2(cAction->sXY.y, cAction->sXY.x);
		cAction->sXY = { cos(_fDirectionRadian) , sin(_fDirectionRadian) }; // ������ x, y ��ǥ�� ������Ƿ� ����ȭ �Ǿ��ִ�.
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