#include "stdafx.h"
#include "lib.h"
#include "Physics.h"

const char *Const::szActionTypesAsString[EActionType::EActionType_Count] = { "EActionType_Idle" , "EActionType_MoveTo", "EActionType_Shoot" };
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
	CAction curAction = cActionList.GetCurAction();
	const vector<CSpriteInfo> * spriteInfos = &arAniInfos[(int)curAction.eActionType].SpriteInfos;
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
	if (this->bControlled) {
		CUnit *otherUnit = Physics::hitTest(*this);
		if (otherUnit) {
			this->cSubUnit = otherUnit;
			//CAction cAction;
			//cAction.eActionType = EActionType::EActionType_MoveTo;
			//cAction.sXY = { 1.0, 0.0 };
			////cAction.sXY = { otherUnit->sXY.x + cAction.sXY.x * this->fSpeedPerSeconds * fDeltaTime * Const::fSpeedPerFrameMagnification(),
			//cAction.sXY = { 800,
			//	otherUnit->sXY.y + cAction.sXY.y * this->fSpeedPerSeconds * fDeltaTime * Const::fSpeedPerFrameMagnification() };
			//otherUnit->cActionList.Clear();
			//otherUnit->cActionList.AddAction(cAction);
		}
	}
	//sub unit
	if (cSubUnit != nullptr) {
		CAction cAction;
		cAction.eActionType = EActionType::EActionType_MoveTo;
		cAction.sXY = { 1.0, 0.0 };
		//cAction.sXY = { otherUnit->sXY.x + cAction.sXY.x * this->fSpeedPerSeconds * fDeltaTime * Const::fSpeedPerFrameMagnification(),
		cAction.sXY = this->sXY;
		cAction.sXY.Add(100, 0);
		cSubUnit->cActionList.Clear();
		cSubUnit->cActionList.AddAction(cAction);
	}
	// �׼� ���� ����1
	if (bEndAni && !curAction.bRepeat) {
		// ani�� ������ �ѹ��� ����ϴ� �׼��̶�� �׼� �����ϰ� ��������Ʈ ���� �ٽ� �б�
		NextAction();
		curAction = cActionList.GetCurAction();
		spriteInfos = &arAniInfos[(int)curAction.eActionType].SpriteInfos;
		_cCurSpriteInfo = (*spriteInfos)[_iAniIndex];
	}
	// �׼� ���� ����2
	if (curAction.eActionType == EActionType::EActionType_Idle) {
		if (_iWaitTimeOnPosition == 0) {
			_iWaitTimeOnPosition = GetTickCount();
			//return;
		}
		else {
			if (curAction.iTime != INT_MAX && GetTickCount() - _iWaitTimeOnPosition >= curAction.iTime) {
				// end wait
				_iWaitTimeOnPosition = 0;
				NextAction();
			}
			else {
				// wait
				//return;
			}
		}
	}
	else if (curAction.eActionType == EActionType::EActionType_MoveTo) {
		// ���� ���������� �Ÿ�
		SXY distanceXY = curAction.sXY - sXY;
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

		if (sameXY(curAction.sXY, sXY)) {
			NextAction();
		}
	}
	else if (curAction.eActionType == EActionType::EActionType_Shoot) {
		if (this->bControlled && cSubUnit != nullptr) {
			CAction cAction;
			cAction.eActionType = EActionType::EActionType_MoveTo;
			cAction.sXY = {800, this->sXY.y};
			cSubUnit->cActionList.Clear();
			cSubUnit->cActionList.AddAction(cAction);
			cSubUnit = nullptr;
		}
		/*vector<CSpriteInfo> &spriteInfos = arAniInfos[(int)curAction.eActionType].SpriteInfos;
		if (bEndAni) {
			NextAction();
		}*/
	}
}
void CUnit::Render(HDC hdc) {
	//if (!bPlaying) return;
	// test
	//fWH g_whBottomWndSize = { 800, 600 };
	// ȭ�� ���߾� ��ǥ
	//SXY center = { g_whBottomWndSize.w / 2, g_whBottomWndSize.h / 2 };

	// ���õ� ��������Ʈ�� ũ��
	fWH spriteSize = { _cCurSpriteInfo.sRect.right - _cCurSpriteInfo.sRect.left , _cCurSpriteInfo.sRect.bottom - _cCurSpriteInfo.sRect.top };
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
	// test : text radian
	char szDirection[FLT_MAX_10_EXP];
	sprintf_s(szDirection, FLT_MAX_10_EXP, "%f", _fDirectionRadian);
	TextOut(hdc, 0, 0, szDirection, FLT_MAX_10_EXP);
	// test : text direction
	sprintf_s(szDirection, FLT_MAX_10_EXP, "%f", _fDirectionWithCosRight);
	TextOut(hdc, 0, 100, szDirection, FLT_MAX_10_EXP);
	if (_fDirectionWithCosRight > 0) {
		sprintf_s(szDirection, FLT_MAX_10_EXP, "%s", "right");
	}
	else {
		sprintf_s(szDirection, FLT_MAX_10_EXP, "%s", "left");
	}
	TextOut(hdc, 0, 200, szDirection, FLT_MAX_10_EXP);
	// test : text _iAniIndex
	CAction curAction = cActionList.GetCurAction();
	vector<CSpriteInfo> &spriteInfos = arAniInfos[(int)curAction.eActionType].SpriteInfos;
	sprintf_s(szDirection, FLT_MAX_10_EXP, "%d/%d", _iAniIndex, spriteInfos.size());
	TextOut(hdc, sXY.x, sXY.y + 2, szDirection, FLT_MAX_10_EXP);
}
void CUnit::Reset() {
	_iAniIndex = 0;
	_iAniTime = GetTickCount();
	// �ʱ� ��ġ ����
	cActionList.Init();
	cActionListPattern.Init();
	cActionList = cActionListPattern;
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
		this->cFilePath = *cFilePath;
		char szLine[Const::szMax_ItemLine];
		// name
		Func::FGetStr(szName, szMax_UnitName, file);
		//fMagnification
		Func::FGetStr(szLine, FLT_MAX_10_EXP, file);
		fMagnification = atof(szLine);
		//fSpeedPerSeconds
		Func::FGetStr(szLine, FLT_MAX_10_EXP, file);
		fSpeedPerSeconds = atof(szLine);
		// bitmap file path
		Func::FGetStr(szBitmapPath, MAX_PATH, file);
		// pattern file path
		Func::FGetStr(cActionList.szFilePath, MAX_PATH, file);
		// ani files
		Func::FGetStr(szLine, Const::szMax_ItemLine, file);
		int itemCount = atoi(szLine);
		for (size_t i = 0; i < itemCount; i++) {
			// ===== List�� ������ �߰� =====
			memset(szLine, 0, Const::szMax_ItemLine);

			Func::FGetStr(szLine, Const::szMax_ItemLine, file);

			char *token;
			char *nextToken;
			nextToken = szLine;
			SAniInfo aniInfo;
			token = strtok_s(nullptr, "\t", &nextToken);
			strcpy_s(aniInfo.FilePath, MAX_PATH, token);
			token = strtok_s(nullptr, "\t", &nextToken);
			strcpy_s(aniInfo.FileTitle, MAX_PATH, token);

			arAniInfos[i] = aniInfo;
		}
		fclose(file);
	}
	// load bitmap
	LoadUnitBitmap(szBitmapPath);
	// load .usp
	cActionListPattern.LoadActionPatternFile(cActionList.szFilePath);
	cActionList = cActionListPattern;
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