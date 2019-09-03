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
#define szMax_UnitState 9 + 1
#define szMax_PosLine 99
#define nFrameRate 60 // ����� �ֻ����� 60hz�ε� 60���� ���̸� �� �ε巴�� ���δ�. ���ϱ�?
#define nPivotHalfSize 5
#define nMax_SpriteCollision 4
#define nMax_SpriteCoordinateCount (4/*sprite*/ + 2/*pivot*/)
#define nMax_SpriteCoordinateByteSize (sizeof(int) * nMax_SpriteCoordinateCount)
#define nMax_RectPos 4
#define szMax_Magnification 3 + 1

#define szMax_UnitStateLine 99
#define szMax_UnitName 99

#define nMax_UnitStateCount 999
#define szMax_UnitStateCount 3 + 1

#define NoSelectedIndex -1

using namespace std;

// todo : define�� ��ü�ϱ�
static class Const {
public:
	const static int szMax_Path = MAX_PATH;
	const static int szMax_ItemCount = _MAX_INT_DIG;
	const static int szMax_ListColumnName = 32;
	const static int szMax_ItemLine = MAX_PATH;

	const static int nMax_ListColumnWidth = 100;
};

// ===== enum =====
enum EWindowMode {
	None, Window, FullScreen
};
enum EUnitStateType {
	EUnitStateType_None = -1,
	EUnitStateType_Idle = 0, EUnitStateType_MoveTo, EUnitStateType_Shoot,
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
void dlog(float i, float ii, float iii, float iiii);
// ===== inline function ===== end

// ===== class ===== 
class CFilePath {
public:
	char szFilePath[Const::szMax_Path];
	char szFileTitle[Const::szMax_Path];
	void Clear() {
		szFilePath[0] = 0;
		szFileTitle[0] = 0;
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

class CUnitState {
public:
	EUnitStateType eUnitStateType;
	SXY sXY;
	UINT iTime; // milliseconds

	CUnitState() {
	}
};

class CUnitStatePattern {
public:
	CUnitState cDefaultUnitState;
	deque<CUnitState> vecCUnitState;
	UINT iUnitStateIndex;
	char szFilePath[MAX_PATH];
	char szFileTitle[MAX_PATH];

	CUnitStatePattern() {
		cDefaultUnitState;
		cDefaultUnitState.eUnitStateType == EUnitStateType::EUnitStateType_Idle;
		cDefaultUnitState.iTime == INT_MAX;
	}
	~CUnitStatePattern() {
	}

	void Init() {
		//iUnitStateIndex = 0;
	}
	CUnitState& GetCurUnitState() {
		if (vecCUnitState.size() == 0) {
			return cDefaultUnitState;
		}
		return vecCUnitState[0];
	}
	/*CUnitState& GetCurUnitState() {
		if (iUnitStateIndex < 0 || iUnitStateIndex >= vecCUnitState.size()) {
			return cDefaultUnitState;
		}
		return vecCUnitState[iUnitStateIndex];
	}*/
	void UpUnitStateIndex() {
		vecCUnitState.pop_front();
		//++iUnitStateIndex;

		//if (iUnitStateIndex == vecCUnitState.size()) {
		//	iUnitStateIndex = 0;
		//}
	}
	void AddUnitState(CUnitState unitState) {
		vecCUnitState.push_back(unitState);
	}
	bool DeleteUnitState(UINT index) {
		if (index >= vecCUnitState.size()) return false;

		auto iter = vecCUnitState.begin();
		vecCUnitState.erase(iter + index);

		return true;
	}
	bool DeleteAllUnitState() {
		vecCUnitState.clear();

		return true;
	}
	void Clear() {
		Init();

		DeleteAllUnitState();
		szFilePath[0] = 0;
		szFileTitle[0] = 0;
	}
	bool UpUnitState(UINT index) {
		if (index == 0 || index >= vecCUnitState.size()) return false;

		CUnitState unitStateTemp = vecCUnitState[index - 1];
		vecCUnitState[index - 1] = vecCUnitState[index];
		vecCUnitState[index] = unitStateTemp;

		return true;
	}
	bool DownUnitState(UINT index) {
		if (index < 0 || index >= vecCUnitState.size() - 1) return false;

		CUnitState unitStateTemp = vecCUnitState[index + 1];
		vecCUnitState[index + 1] = vecCUnitState[index];
		vecCUnitState[index] = unitStateTemp;

		return true;
	}
	void LoadUnitStatePatternFile(const char *filePath) {
		FILE *file = nullptr;
		file = _fsopen(filePath, "rt", _SH_DENYNO);

		if (file == nullptr) return;

		strcpy_s(szFilePath, MAX_PATH, filePath);
		strcpy_s(szFileTitle, MAX_PATH, GetFileNameByFullPath(filePath));

		char szItemCount[szMax_SpriteCount] = {};
		fgets(szItemCount, szMax_SpriteCount, file);

		// item count
		int itemCount = atoi(szItemCount);

		vecCUnitState.clear();
		//vecCUnitState.reserve(itemCount);

		// ===== List�� ������ �߰� =====
		char itemLine[szMax_UnitStateLine] = {};
		char *token;
		char *nextToken;
		for (size_t i = 0; i < itemCount; i++) {
			memset(itemLine, 0, szMax_UnitStateLine);
			fgets(itemLine, szMax_UnitStateLine, file);

			// \n�� �ٹٲ��� �����ϴ� �����̹Ƿ� ���� ���ڸ� ��� ���� �����Ѵ�.
			itemLine[strcspn(itemLine, "\n")] = 0; // strcspn()���� "\n"�� ��ġ�� ã�� �� ��ġ�� 0�� �־��ش�.

			if (strnlen_s(itemLine, szMax_PosLine) == 0) continue;

			CUnitState unitState;
			{
				nextToken = itemLine;

				token = strtok_s(nullptr, "\t", &nextToken);
				unitState.eUnitStateType = (EUnitStateType)atoi(token);
				token = strtok_s(nullptr, "\t", &nextToken);
				unitState.sXY.x = atof(token);
				token = strtok_s(nullptr, "\t", &nextToken);
				unitState.sXY.y = atof(token);
				token = strtok_s(nullptr, "\t", &nextToken);
				unitState.iTime = atoi(token);
			}
			AddUnitState(unitState);
		}
		fclose(file);
	}
	void SaveUnitStatePatternFile(const char *filePath) {
		FILE *file = nullptr;
		file = _fsopen(filePath, "wt", _SH_DENYNO);

		if (file == nullptr) return;

		// item count
		int itemCount = vecCUnitState.size();
		char szItemCount[szMax_UnitStateCount] = {};
		sprintf_s<szMax_UnitStateCount>(szItemCount, "%d\n", itemCount);
		fputs(szItemCount, file);

		auto iter = vecCUnitState.begin();
		while (iter != vecCUnitState.end()) {
			fprintf_s(file, "%d\t%f\t%f\t%d\n", iter->eUnitStateType, iter->sXY.x, iter->sXY.y, iter->iTime);

			++iter;
		}
		fclose(file);
	}
	void RenderUnitState(HDC _hdcMem) {
		if (vecCUnitState.size() == 0) return;

		HPEN hLinePen = (HPEN)GetStockObject(BLACK_PEN);
		HPEN hIdlePen = CreatePen(PS_SOLID, 1, RGB(0, 255, 0));;
		HPEN hWalkPen = CreatePen(PS_SOLID, 1, RGB(0, 0, 255));;
		HPEN hSelectedPen = CreatePen(PS_SOLID, 1, RGB(255, 0, 0));
		HPEN hOldPen = (HPEN)SelectObject(_hdcMem, hWalkPen);

		MoveToEx(_hdcMem, vecCUnitState[0].sXY.x, vecCUnitState[0].sXY.y, nullptr);
		WH wh = { 10, 10 };
		SXY prevXY = { 0.0, 0.0 };
		for (size_t i = 0; i < vecCUnitState.size(); i++) {
			CUnitState unitState = vecCUnitState[i];
			SXY xy = { 0.0, 0.0 };
			float size = 1.0;

			if (unitState.eUnitStateType == EUnitStateType::EUnitStateType_Idle) {
				SelectObject(_hdcMem, hIdlePen);

				xy = prevXY;
				size = 3;
			}
			else if (unitState.eUnitStateType == EUnitStateType::EUnitStateType_MoveTo) {
				SelectObject(_hdcMem, hLinePen);
				LineTo(_hdcMem, unitState.sXY.x, unitState.sXY.y);

				SelectObject(_hdcMem, hWalkPen);

				xy = unitState.sXY;
				prevXY = unitState.sXY;
				size = 2;
			}

			Ellipse(_hdcMem, prevXY.x - wh.w / size, prevXY.y - wh.h / size,
				prevXY.x + wh.w / size, prevXY.y + wh.w / size);

			if (i == 0) {
				SelectObject(_hdcMem, hSelectedPen);
				Rectangle(_hdcMem, xy.x - wh.w / 2, xy.y - wh.h / 2,
					xy.x + wh.w / 2, xy.y + wh.w / 2);
			}
			SelectObject(_hdcMem, hWalkPen);
		}
		SelectObject(_hdcMem, hOldPen);
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

	bool bInitialized = false;
	char szName[szMax_UnitName];
	SXY sXY;
	WH sWH;
	float fSpeedPerSeconds = 10.0;
	float fMagnification = 1.0;
	EUnitStateType eCurUnitStateType;
	char szBitmapPath[MAX_PATH];
	SAniInfo arAniInfos[EUnitStateType::Count];
	CUnitStatePattern cUnitStateAction;
	CUnitStatePattern cUnitStatePattern;
	HDC hBitmapDC;

private:
	bool _bPatternPlaying = false;
	// direction
	float _fDirectionRadian = 0.0;
	V2 _v2NormalDirection = {};
	float _fDirectionWithCosRight = 0.0;
	// ani
	time_t _iAniTime = 0;
	UINT _iAniIndex = 0;
	CSpriteInfo _cCurSpriteInfo;
	// pos
	time_t _iWaitTimeOnPosition = 0;

public:
	CUnit() {
		for (size_t i = 0; i < EUnitStateType::Count; i++) {
			arAniInfos[i].FilePath[0] = 0;
			arAniInfos[i].FileTitle[0] = 0;
		}
	}
	~CUnit() {}

	void Init(HDC hdc) {
		if (hBitmapDC) {
			DeleteDC(hBitmapDC);
		}
		hBitmapDC = CreateCompatibleDC(hdc);
	}
	void Update(float fDeltaTime) {
		// update CUnitState
		CUnitState curUnitState = cUnitStateAction.GetCurUnitState();

		if (curUnitState.eUnitStateType == EUnitStateType::EUnitStateType_Idle) {
			if (_iWaitTimeOnPosition == 0) {
				_iWaitTimeOnPosition = GetTickCount();
				//return;
			}
			else {
				if (GetTickCount() - _iWaitTimeOnPosition >= curUnitState.iTime) {
					// end wait
					_iWaitTimeOnPosition = 0;
					NextUnitState();
				}
				else {
					// wait
					//return;
				}
			}
		}
		else if (curUnitState.eUnitStateType == EUnitStateType::EUnitStateType_MoveTo) {
			// ���� ���������� �Ÿ�
			SXY distanceXY = curUnitState.sXY - sXY;
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

			if (sameXY(curUnitState.sXY, sXY)) {
				NextUnitState();
				//dlog(curUnitState.sXY.x, curUnitState.sXY.y, sXY.x, sXY.y);
			}
		}
		else if (curUnitState.eUnitStateType == EUnitStateType::EUnitStateType_Shoot) {
			vector<CSpriteInfo> &spriteInfos = arAniInfos[(int)curUnitState.eUnitStateType].SpriteInfos;
			if (spriteInfos.size() - 1 == _iAniIndex) {
				NextUnitState();
			}
		}

		// update ani
		time_t time = GetTickCount();
		if (time - _iAniTime >= _cCurSpriteInfo.iTime) {
			_iAniTime = time;
			vector<CSpriteInfo> &spriteInfos = arAniInfos[(int)curUnitState.eUnitStateType].SpriteInfos;
			size_t size = spriteInfos.size();
			if (size != 0) {
				++_iAniIndex;
				if (_iAniIndex >= spriteInfos.size()) {
					_iAniIndex = 0;
				}
				_cCurSpriteInfo = spriteInfos[_iAniIndex];
			}
		}
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
	}
	void Reset() {
		_iAniIndex = 0;
		_iAniTime = GetTickCount();
		// �ʱ� ��ġ ����
		cUnitStateAction.Init();
		CUnitState unitState = cUnitStateAction.GetCurUnitState();
		sXY = unitState.sXY;
	}
	void Clear() {
		eCurUnitStateType = EUnitStateType::EUnitStateType_None;
		_iAniIndex = 0;
		_iAniTime = GetTickCount();
		cUnitStateAction.Clear();
		cUnitStatePattern.Clear();

		szName[0] = 0;
		SXY sXY = {0, 0};
		WH sWH = {0, 0};
		float fSpeedPerSeconds = 10.0;
		float fMagnification = 1.0;
		//EUnitStateType eCurUnitStateType;
		szBitmapPath[0] = 0;
		for (size_t i = 0; i < static_cast<EUnitStateType>(EUnitStateType::Count); i++) {
			arAniInfos[i].FilePath[0] = 0;
			arAniInfos[i].FileTitle[0] = 0;
			arAniInfos[i].SpriteInfos.clear();
		}
		ClearUnitBitmap();

		bInitialized = false;
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
	void LoadAniFile(EUnitStateType unitStateType, const char *filePath) {
		FILE *file = nullptr;
		file = _fsopen(filePath, "rt", _SH_DENYNO);

		if (file == nullptr) return;

		SAniInfo &aniFile = arAniInfos[(int)unitStateType];
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
	void NextUnitState() {
		CUnitState cPrevUnitState = cUnitStateAction.GetCurUnitState();
		cUnitStateAction.UpUnitStateIndex();
		CUnitState cCurUnitState = cUnitStateAction.GetCurUnitState();
		bool bUnitStateChanged = cPrevUnitState.eUnitStateType != cCurUnitState.eUnitStateType;

		if (bUnitStateChanged) {
			if (cCurUnitState.eUnitStateType != EUnitStateType::EUnitStateType_MoveTo) {
				_iAniIndex = 0;
			}
		}
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