#pragma once

#include <vector>
#include <deque>
#include <list>
#include "windows.h"
//#include "windowsx.h"
#include "function.h"
#define _USE_MATH_DEFINES // math.h�� ����ϱ� ���� �ʿ�
#include <math.h>

// ===== define =====
#define nMax_SpriteCount 999
#define szMax_SpriteCount 3 + 1
#define szMax_Pos 6
#define szMax_Action 9 + 1
#define szMax_PosLine 99
#define nFrameRate 60 // ����� �ֻ����� 60hz�ε� 60���� ���̸� �� �ε巴�� ���δ�. ���ϱ�?
#define nPivotHalfSize 5
#define nMax_SpriteCollision 4
#define nMax_SpriteCoordinateCount (4/*sprite*/ + 2/*pivot*/)
#define nMax_SpriteCoordinateByteSize (sizeof(int) * nMax_SpriteCoordinateCount)
#define nMax_RectPos 4
#define szMax_Magnification 3 + 1

#define szMax_ActionLine 99
#define szMax_UnitName 99

#define nMax_ActionCount 999
#define szMax_ActionCount 3 + 1

#define NoSelectedIndex -1

using namespace std;

// todo : define�� ��ü�ϱ�
static class Const {
public:
	// string length
	const static int szMax_Path = MAX_PATH;
	const static int szMax_ItemCount = _MAX_INT_DIG;
	const static int szMax_ListColumnName = 32;
	const static int szMax_ItemLine = MAX_PATH;
	const static int szMax_StageName = MAX_PATH;

	// float
	// static float�� Ŭ���� ���ο��� �ʱ�ȭ �� �� ���� int�� �����ϱ� ������ �Լ��� �����.
	// �����Ӵ� �̵��� �Ÿ���ŭ�� �̵��ϸ� "�̵� -> ���̵� -> �̵�" �̷������� ���̵� ���°� ���� �ִϸ��̼��� �����.
	// ���� ������ �� �̵��Ÿ��� �÷� �� �����ӿ� �̵��� �Ϸ���� ���ϵ����ؼ� �߰��� ���̵� ���°� �����ʰ� �ϱ����� ��.
	//const static int fSpeedPerFrameMagnification = 2;
	static float fSpeedPerFrameMagnification() {
		return 2.0;
	};

	// string
	static const char *szStageSettingFileName() {
		return "stageCreator.settings";
	}
};

// ===== enum =====
enum EWindowMode {
	None, Window, FullScreen
};
enum EActionType {
	EActionType_None = -1,
	EActionType_Idle = 0, EActionType_MoveTo, EActionType_Shoot,
	Count
};
// ===== enum ===== end
// ===== struct =====
// deprecated
typedef struct _XY {
	INT x;
	INT y;
} XY;
typedef struct _V2 {
	float x;
	float y;
} V2;
// deprecated
typedef struct _WH {
	UINT w;
	UINT h;
} WH;
typedef struct _fWH {
	FLOAT w;
	FLOAT h;
} fWH;
class SXY {
public:
	float x;
	float y;

	SXY& Add(float x, float y) {
		this->x += x;
		this->y += y;
		return (*this);
	}
	float distance() {
		return sqrt((x) * (x)+(y) * (y));
	}
	float distance(SXY xy) {
		return sqrt((xy.x - x) * (xy.x - x) + (xy.y - y) * (xy.y - y));
	}
};
// ===== struct ===== end
// ===== inline function =====
void RemoveCarriageReturn(char *sz);
const char * GetFileNameByFullPath(const char *path);
bool sameXY(SXY xy1, SXY xy2);
bool operator==(XY xy1, XY xy2);
SXY operator-(SXY xy1, SXY xy2);
bool operator==(SXY xy1, SXY xy2);
void dlog(LPCSTR lpStr);
void dlog(float i, float ii, float iii, float iiii);
// ===== inline function ===== end

// ===== class ===== 
class CFilePath {
public:
	char szFileTitle[Const::szMax_Path];
	char szFilePath[Const::szMax_Path];
	CFilePath() {
		szFilePath[0] = 0;
		szFileTitle[0] = 0;
	}
	void Clear() {
		szFilePath[0] = 0;
		szFileTitle[0] = 0;
	}
};
static class Func {
public:
	static bool OpenFileDialog(CFilePath *cFilePath, const char *szFilter) {
		OPENFILENAME ofn = { 0, };
		ofn.lStructSize = sizeof(OPENFILENAME);
		ofn.hwndOwner = nullptr;
		ofn.lpstrFilter = szFilter;
		ofn.lpstrFile = cFilePath->szFilePath;
		ofn.lpstrFileTitle = cFilePath->szFileTitle;
		ofn.nMaxFile = MAX_PATH;
		ofn.nMaxFileTitle = MAX_PATH;
		ofn.lpstrTitle = szFilter;
		// �⺻ ���� ����
		//ofn.lpstrInitialDir = defaultPath;
		//ofn.lpstrInitialDir = "C:\\";
		return GetOpenFileName(&ofn);
	}
	static char* __cdecl FGetStr(char *szBuffer, int iBufferSize, FILE *sFile) {
		char *result = fgets(szBuffer, iBufferSize, sFile);
		RemoveCarriageReturn(szBuffer);
		return result;
	}
	static int FGetInt(char *szBuffer, int iBufferSize, FILE *sFile) {
		FGetStr(szBuffer, iBufferSize, sFile);
		return atoi(szBuffer);
	}
	static float FGetFloat(char *szBuffer, int iBufferSize, FILE *sFile) {
		FGetStr(szBuffer, iBufferSize, sFile);
		return atof(szBuffer);
	}
	static void RemoveCarriageReturn(char *sz) {
		// \n�� �ٹٲ��� �����ϴ� �����̹Ƿ� ���� ���ڸ� ��� ���� �����Ѵ�.
		sz[strcspn(sz, "\n")] = 0; // strcspn()���� "\n"�� ��ġ�� ã�� �� ��ġ�� 0�� �־��ش�.
	}
};
class CSpriteInfo {
public:
	UINT iTime = 0;
	INT ariCoordinates[nMax_SpriteCoordinateCount];
	RECT sRect;
	XY sPivot;
	vector<RECT> vecsCollisions;
	UINT iCollisionCount;

	void SetCoordinates(INT coordinates[], UINT count) {
		memcpy(ariCoordinates, coordinates, count);
		int i = 0;
		sRect.left = ariCoordinates[i++];
		sRect.top = ariCoordinates[i++];
		sRect.right = ariCoordinates[i++];
		sRect.bottom = ariCoordinates[i++];
		sPivot.x = ariCoordinates[i++];
		sPivot.y = ariCoordinates[i++];
	}
	void SetPivot(INT x, INT y) {
		int i = 0;
		sRect.left = ariCoordinates[i++];
		sRect.top = ariCoordinates[i++];
		sRect.right = ariCoordinates[i++];
		sRect.bottom = ariCoordinates[i++];
		ariCoordinates[4] = x;
		ariCoordinates[5] = y;
		sPivot.x = x;
		sPivot.y = y;
	}
	void ResetCollisionCount() {
		iCollisionCount = 0;
	}
	void AddCollision(int collisionPos[], int count) {
		if (iCollisionCount >= nMax_SpriteCollision) {
			return;
		}
		RECT collision = { collisionPos[0], collisionPos[1], collisionPos[2], collisionPos[3] };
		++iCollisionCount;
		vecsCollisions.push_back(collision);
	}
	void RemoveCollision(int index) {
		if (index < 0 || index >= iCollisionCount) {
			return;
		}
		vector<RECT>::iterator iter = vecsCollisions.begin();
		vecsCollisions.erase(iter + index);
		--iCollisionCount;
	}
	void RemoveAllCollisions() {
		if (iCollisionCount == 0) {
			return;
		}
		vecsCollisions.clear();
		iCollisionCount = 0;
	}
};

class CAction {
public:
	EActionType eActionType;
	SXY sXY;
	UINT iTime; // milliseconds
	bool bCancelable;
	bool bOncePlay;

	CAction() {
	}
};

class CActionList {
public:
	CAction cDefaultAction;
	deque<CAction> cActions;
	char szFilePath[MAX_PATH];
	char szFileTitle[MAX_PATH];

	CActionList() {
		cDefaultAction;
		cDefaultAction.eActionType = EActionType::EActionType_Idle;
		cDefaultAction.iTime = UINT_MAX;
		cDefaultAction.bCancelable = true;
		cDefaultAction.bOncePlay = false;
	}
	~CActionList() {
	}
	void Init() {
	}
	CAction& GetCurAction() {
		if (cActions.size() == 0) {
			return cDefaultAction;
		}
		return cActions[0];
	}
	void NextAction() {
		cActions.pop_front();
	}
	void AddAction(CAction action) {
		cActions.push_back(action);
	}
	bool DeleteAction(UINT index) {
		if (index >= cActions.size()) return false;

		auto iter = cActions.begin();
		cActions.erase(iter + index);

		return true;
	}
	bool DeleteAllActions() {
		cActions.clear();

		return true;
	}
	void Clear() {
		Init();
		DeleteAllActions();
		szFilePath[0] = 0;
		szFileTitle[0] = 0;
	}
	bool UpAction(UINT index) {
		if (index == 0 || index >= cActions.size()) return false;

		CAction cActionTemp = cActions[index - 1];
		cActions[index - 1] = cActions[index];
		cActions[index] = cActionTemp;

		return true;
	}
	bool DownAction(UINT index) {
		if (index < 0 || index >= cActions.size() - 1) return false;

		CAction cActionTemp = cActions[index + 1];
		cActions[index + 1] = cActions[index];
		cActions[index] = cActionTemp;

		return true;
	}
	void LoadActionPatternFile(const char *filePath) {
		FILE *file = nullptr;
		file = _fsopen(filePath, "rt", _SH_DENYNO);

		if (file == nullptr) return;

		strcpy_s(szFilePath, MAX_PATH, filePath);
		strcpy_s(szFileTitle, MAX_PATH, GetFileNameByFullPath(filePath));

		char szItemCount[szMax_SpriteCount] = {};
		fgets(szItemCount, szMax_SpriteCount, file);

		// item count
		int itemCount = atoi(szItemCount);

		cActions.clear();
		//cActionList.reserve(itemCount);

		// ===== List�� ������ �߰� =====
		char itemLine[szMax_ActionLine] = {};
		char *token;
		char *nextToken;
		for (size_t i = 0; i < itemCount; i++) {
			memset(itemLine, 0, szMax_ActionLine);
			fgets(itemLine, szMax_ActionLine, file);

			// \n�� �ٹٲ��� �����ϴ� �����̹Ƿ� ���� ���ڸ� ��� ���� �����Ѵ�.
			itemLine[strcspn(itemLine, "\n")] = 0; // strcspn()���� "\n"�� ��ġ�� ã�� �� ��ġ�� 0�� �־��ش�.

			if (strnlen_s(itemLine, szMax_PosLine) == 0) continue;

			CAction cAction;
			nextToken = itemLine;
			token = strtok_s(nullptr, "\t", &nextToken);
			cAction.eActionType = (EActionType)atoi(token);
			token = strtok_s(nullptr, "\t", &nextToken);
			cAction.sXY.x = atof(token);
			token = strtok_s(nullptr, "\t", &nextToken);
			cAction.sXY.y = atof(token);
			token = strtok_s(nullptr, "\t", &nextToken);
			cAction.iTime = atoi(token);
			AddAction(cAction);
		}
		fclose(file);
	}
	void SaveActionPatternFile(const char *filePath) {
		FILE *file = nullptr;
		file = _fsopen(filePath, "wt", _SH_DENYNO);

		if (file == nullptr) return;

		// item count
		int itemCount = cActions.size();
		char szItemCount[szMax_ActionCount] = {};
		sprintf_s<szMax_ActionCount>(szItemCount, "%d\n", itemCount);
		fputs(szItemCount, file);

		auto iter = cActions.begin();
		while (iter != cActions.end()) {
			fprintf_s(file, "%d\t%f\t%f\t%d\n", iter->eActionType, iter->sXY.x, iter->sXY.y, iter->iTime);

			++iter;
		}
		fclose(file);
	}
	void RenderActions(HDC hdcMem) {
		if (cActions.size() == 0) return;

		HPEN hLinePen = (HPEN)GetStockObject(BLACK_PEN);
		HPEN hIdlePen = CreatePen(PS_SOLID, 1, RGB(0, 255, 0));;
		HPEN hWalkPen = CreatePen(PS_SOLID, 1, RGB(0, 0, 255));;
		HPEN hSelectedPen = CreatePen(PS_SOLID, 1, RGB(255, 0, 0));
		HPEN hOldPen = (HPEN)SelectObject(hdcMem, hWalkPen);

		MoveToEx(hdcMem, cActions[0].sXY.x, cActions[0].sXY.y, nullptr);
		WH wh = { 10, 10 };
		SXY prevXY = { 0.0, 0.0 };
		for (size_t i = 0; i < cActions.size(); i++) {
			CAction cAction = cActions[i];
			SXY xy = { 0.0, 0.0 };
			float size = 1.0;

			if (cAction.eActionType == EActionType::EActionType_Idle) {
				SelectObject(hdcMem, hIdlePen);

				xy = prevXY;
				size = 3;
			}
			else if (cAction.eActionType == EActionType::EActionType_MoveTo) {
				SelectObject(hdcMem, hLinePen);
				LineTo(hdcMem, cAction.sXY.x, cAction.sXY.y);

				SelectObject(hdcMem, hWalkPen);

				xy = cAction.sXY;
				prevXY = cAction.sXY;
				size = 2;
			}

			Ellipse(hdcMem, prevXY.x - wh.w / size, prevXY.y - wh.h / size,
				prevXY.x + wh.w / size, prevXY.y + wh.w / size);

			if (i == 0) {
				SelectObject(hdcMem, hSelectedPen);
				Rectangle(hdcMem, xy.x - wh.w / 2, xy.y - wh.h / 2,
					xy.x + wh.w / 2, xy.y + wh.w / 2);
			}
			SelectObject(hdcMem, hWalkPen);
		}
		SelectObject(hdcMem, hOldPen);
		DeleteObject(hLinePen);
		DeleteObject(hIdlePen);
		DeleteObject(hWalkPen);
		DeleteObject(hSelectedPen);
	}
};

struct SAniInfo {
	SAniInfo() {}
	~SAniInfo() {}

	char FilePath[MAX_PATH];
	char FileTitle[MAX_PATH];
	vector<CSpriteInfo> SpriteInfos;
};

class CUnit {
public:
	const V2 kV2Right = { 1, 0 };

	CFilePath cFilePath;
	bool bInitialized = false;
	char szName[szMax_UnitName];
	SXY sStartXY = {0.0, 0.0};
	SXY sXY = {0.0, 0.0};
	WH sWH;
	float fSpeedPerSeconds = 10.0;
	float fMagnification = 1.0;
	char szBitmapPath[MAX_PATH];
	SAniInfo arAniInfos[EActionType::Count];
	CActionList cActionList;
	CActionList cActionListPattern;
	HDC hBitmapDC;
	UINT _iAniIndex = 0;

private:
	bool _bPatternPlaying = false;
	// direction
	float _fDirectionRadian = 0.0;
	V2 _v2NormalDirection = {};
	float _fDirectionWithCosRight = 0.0;
	// ani
	time_t _iAniTime = 0;
	CSpriteInfo _cCurSpriteInfo;
	// pos
	time_t _iWaitTimeOnPosition = 0;

public:
	CUnit() {
		for (size_t i = 0; i < EActionType::Count; i++) {
			arAniInfos[i].FilePath[0] = 0;
			arAniInfos[i].FileTitle[0] = 0;
		}
	}
	~CUnit() {}
	CUnit& operator=(const CUnit& r) {
		//������ �Ǵ� �������̷� ���� ��ü�� �����Ͱ� �޸𸮻� �������� �ƴ� ��� �ϰ������� memcpy�� ����ϸ� �ȵǰ� ���� ����������Ѵ�.
		//memcpy(this, &r, sizeof(*this));

		memcpy(&this->cFilePath, &r.cFilePath, sizeof(this->cFilePath));

		for (size_t i = 0; i < EActionType::Count; i++) {
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
		this->_cCurSpriteInfo.vecsCollisions = r._cCurSpriteInfo.vecsCollisions;

		return (*this);
	}

	void Init(HDC hdc) {
		if (hBitmapDC) {
			DeleteDC(hBitmapDC);
		}
		hBitmapDC = CreateCompatibleDC(hdc);
	}
	void Update(float fDeltaTime) {
		CAction curAction = cActionList.GetCurAction();
		const vector<CSpriteInfo> * spriteInfos = &arAniInfos[(int)curAction.eActionType].SpriteInfos;
		_cCurSpriteInfo = (*spriteInfos)[_iAniIndex]; // todo : to pointer
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

		// �׼� ���� ����1
		if (bEndAni && curAction.bOncePlay) {
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
		//else if (curAction.eActionType == EActionType::EActionType_Shoot) {
		//	vector<CSpriteInfo> &spriteInfos = arAniInfos[(int)curAction.eActionType].SpriteInfos;
		//	if (bEndAni) {
		//		NextAction();
		//	}
		//}
	}
	void Render(HDC hdc) {
		//if (!_bPlaying) return;
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
	void Reset() {
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
	void ClearAni() {
		_iAniIndex = 0;
		//_iAniTime = 0;
		_iAniTime = GetTickCount();
	}
	void Clear() {
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
		for (size_t i = 0; i < static_cast<EActionType>(EActionType::Count); i++) {
			arAniInfos[i].FilePath[0] = 0;
			arAniInfos[i].FileTitle[0] = 0;
			arAniInfos[i].SpriteInfos.clear();
		}
		ClearUnitBitmap();

		bInitialized = false;
	}
	void LoadUnit(CFilePath *cFilePath) {
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
		for (size_t i = 0; i < EActionType::Count; i++) {
			LoadAniFile((EActionType)i, arAniInfos[i].FilePath);
		}

		bInitialized = true;
	}
	void SetName(const char *name) {
		strcpy_s(szName, szMax_UnitName, name);
	}
	void LoadUnitBitmap(const char *filePath) {
		strcpy_s(szBitmapPath, MAX_PATH, filePath);
		HBITMAP hBitmap = (HBITMAP)LoadImage(nullptr, filePath, IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
		SelectObject(hBitmapDC, hBitmap); // HBITMAP�� HDC�� ����Ǹ� �ٽ� ����� �� ���� ������ ������ ���� HDC�� �־�д�.
		DeleteObject(hBitmap);
	}
	void ClearUnitBitmap() {
		HBITMAP hBitmap = CreateCompatibleBitmap(hBitmapDC, 100, 100);
		SelectObject(hBitmapDC, hBitmap); // HBITMAP�� HDC�� ����Ǹ� �ٽ� ����� �� ���� ������ ������ ���� HDC�� �־�д�.
		DeleteObject(hBitmap);
	}
	void LoadAniFile(EActionType eActionType, const char *filePath) {
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
	void NextAction() {
		cActionList.NextAction();
		_iAniIndex = 0;
	}
};
// ===== class ===== end
// ===== function =====
inline WH RectToWH(RECT rect) {
	int w = rect.right - rect.left;
	int h = rect.bottom - rect.top;
	WH wh = { w, h };
	return wh;
}
// \n ����
inline void RemoveCarriageReturn(char *sz) {
	// \n�� �ٹٲ��� �����ϴ� �����̹Ƿ� ���� ���ڸ� ��� ���� �����Ѵ�.
	sz[strcspn(sz, "\n")] = 0; // strcspn()���� "\n"�� ��ġ�� ã�� �� ��ġ�� 0�� �־��ش�.
}
// ���� ��ο��� ���ϸ� ���� ���ڿ��� �����͸� ��ȯ
inline const char * GetFileNameByFullPath(const char *path) {
	auto szFileName = strrchr(path, '/'); // strrchr() ���������� ��ġ�ϴ� ������ �����͸� ��ȯ
	if (szFileName == nullptr) {
		szFileName = strrchr(path, '\\'); // strrchr() ���������� ��ġ�ϴ� ������ �����͸� ��ȯ
	}

	if (szFileName != nullptr) {
		++szFileName; // '/' or '\\' ����
	}
	return szFileName;
}
// ===== function ===== end
// ===== global operation overloading ===== 
inline RECT operator*(RECT rect, float v) {
	rect.top *= v;
	rect.left *= v;
	rect.right *= v;
	rect.bottom *= v;
	return rect;
}
inline RECT operator/(RECT rect, float v) {
	rect.top /= v;
	rect.left /= v;
	rect.right /= v;
	rect.bottom /= v;
	return rect;
}
inline RECT operator+(RECT rect, int v) {
	rect.top += v;
	rect.left += v;
	rect.right += v;
	rect.bottom += v;
	return rect;
}
inline RECT operator+(RECT rect, POINT pnt) {
	rect.top += pnt.y;
	rect.left += pnt.x;
	rect.right += pnt.x;
	rect.bottom += pnt.y;
	return rect;
}
inline RECT operator-(RECT rect, POINT pnt) {
	rect.top -= pnt.y;
	rect.left -= pnt.x;
	rect.right -= pnt.x;
	rect.bottom -= pnt.y;
	return rect;
}
inline XY operator-(XY xy1, XY xy2) {
	xy1.x -= xy2.x;
	xy1.y -= xy2.y;
	return xy1;
}
inline XY operator-(XY xy, POINT pnt) {
	xy.x -= pnt.x;
	xy.y -= pnt.y;
	return xy;
}
inline XY operator*(XY xy, float v) {
	xy.x *= v;
	xy.y *= v;
	return xy;
}
inline bool operator==(XY xy1, XY xy2) {
	return (xy1.x == xy2.x) && (xy1.y == xy2.y);
}
inline SXY operator-(SXY xy1, SXY xy2) {
	xy1.x -= xy2.x;
	xy1.y -= xy2.y;
	return xy1;
}
inline bool operator==(SXY xy1, SXY xy2) {
	return (xy1.x == xy2.x) && (xy1.y == xy2.y);
}
inline bool sameXY(SXY xy1, SXY xy2) {
	return (abs(xy1.x - xy2.x) < 0.1) && (abs(xy1.y - xy2.y) < 0.1);
}
// ===== operation overloading ===== end
// ===== log =====
// for string
inline void _dlog(UINT count, const char *args, ...) {
	va_list ap; // �������� �޸� �ּ�
	va_start(ap, args); // �������� �޸� �ּ� ����
	OutputDebugString(args); // ������ �ִ� ���� �׳� ���
	for (size_t i = 0; i < count - 1; i++) {
		OutputDebugString(", ");
		char *str = va_arg(ap, char *); // �������ڴ� va_list���� �ڷ��� ũ�⸸ŭ �̵��� ���� ���� ���
		OutputDebugString(str);

	}
	OutputDebugString("\n");
	va_end(ap);
};
// for int
inline void _dlog(const char *title, UINT count, int args, ...) {
	va_list ap; // �������� �޸� �ּ�
	va_start(ap, args); // �������� �޸� �ּ� ����
	// ���� ���
	if (title[0] != '\0') {
		OutputDebugString(title); // ������ �ִ� ���� �׳� ���
		OutputDebugString(" : "); // ������ �ִ� ���� �׳� ���
	}
	// ù��° ���� ���
	{
		char buffer[11];
		sprintf_s(buffer, 11, "%d", args);
		OutputDebugString(buffer);
	}
	// �������� ���
	for (size_t i = 0; i < count - 1; i++) {
		// char, short, int�� int Ÿ�� ũ��� �̵�
		// �������ڴ� va_list���� �ڷ��� ũ�⸸ŭ �̵��� ���� ���� ���
		int arg = va_arg(ap, int);
		OutputDebugString(", ");
		char buffer[11];
		sprintf_s(buffer, 11, "%d", arg);
		OutputDebugString(buffer);
	}
	OutputDebugString("\n");
	va_end(ap);
};
// for float
inline void _dlog(const char *title, UINT count, float args, ...) {
	va_list ap; // �������� �޸� �ּ�
	va_start(ap, args); // �������� �޸� �ּ� ����
	// ���� ���
	OutputDebugString(title); // ������ �ִ� ���� �׳� ���
	OutputDebugString(" : ");
	// ù��° ���� ���
	{
		char buffer[FLT_MAX_10_EXP];
		sprintf_s(buffer, FLT_MAX_10_EXP, "%f", args);
		OutputDebugString(buffer);
	}
	// �������� ���
	for (size_t i = 0; i < count - 1; i++) {
		// float�� double Ÿ�� ũ��� �̵�
		// https://stackoverflow.com/questions/11270588/variadic-function-va-arg-doesnt-work-with-float
		float arg = va_arg(ap, double);
		OutputDebugString(", ");
		char buffer[FLT_MAX_10_EXP];
		sprintf_s(buffer, FLT_MAX_10_EXP, "%f", arg);
		OutputDebugString(buffer);
	}
	OutputDebugString("\n");
	va_end(ap);
};
inline void dlog(LPCSTR lpStr) {
	_dlog(1, lpStr);
};
inline void dlog(LPCSTR lpStr, LPCSTR lpStr2) {
	_dlog(2, lpStr, lpStr2);
};
inline void dlog(LPCSTR lpStr, LPCSTR lpStr2, LPCSTR lpStr3) {
	_dlog(3, lpStr, lpStr2, lpStr3);
};
inline void dlog(LPCSTR title, int i) {
	_dlog(title, 1, i);
};
inline void dlog(LPCSTR title, int i, int ii) {
	_dlog(title, 2, i, ii);
};
inline void dlog(LPCSTR title, int i, int ii, int iii) {
	_dlog(title, 3, i, ii, iii);
};
inline void dlog(int i) {
	_dlog("", 1, i);
};
inline void dlog(int i, int ii) {
	_dlog("", 2, i, ii);
};
inline void dlog(int i, int ii, int iii) {
	_dlog("", 3, i, ii, iii);
};
inline void dlog(float i) {
	_dlog("", 1, i);
};
inline void dlog(float i, float ii) {
	_dlog("", 2, i, ii);
};
inline void dlog(float i, float ii, float iii) {
	_dlog("", 3, i, ii, iii);
};
inline void dlog(float i, float ii, float iii, float iiii) {
	_dlog("", 4, i, ii, iii, iiii);
};
inline void dlog(RECT rect) {
	_dlog("", 4, rect.left, rect.top, rect.right, rect.bottom);
};
inline void dlog(const char *title, RECT rect) {
	_dlog(title, 4, rect.left, rect.top, rect.right, rect.bottom);
};
// ===== log ===== end