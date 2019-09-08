#pragma once

#include <vector>
#include <deque>
#include <list>
#include "windows.h"
//#include "windowsx.h"
#include "function.h"
#define _USE_MATH_DEFINES // math.h�� ����ϱ� ���� �ʿ�
#include <math.h>
//#include "Physics.h"

// ===== define =====
#define nMax_SpriteCount 999
#define szMax_SpriteCount 3 + 1
#define szMax_Pos 6
#define szMax_Action 99
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


// ===== enum =====
enum EWindowMode {
	EWindowMode_None, EWindowMode_Window, EWindowMode_FullScreen
};
enum EActionType {
	EActionType_None = -1,
	EActionType_Idle = 0, EActionType_MoveTo, EActionType_Shoot,
	Count
};
// ===== enum ===== end

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
	static const char *szActionTypesAsString[EActionType::Count];
	static const char *szStageSettingFileName;
};
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
	CFilePath();
	void Clear();
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
	vector<RECT> sLocalCollisions;
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
		sLocalCollisions.push_back(collision);
	}
	void RemoveCollision(int index) {
		if (index < 0 || index >= iCollisionCount) {
			return;
		}
		vector<RECT>::iterator iter = sLocalCollisions.begin();
		sLocalCollisions.erase(iter + index);
		--iCollisionCount;
	}
	void RemoveAllCollisions() {
		if (iCollisionCount == 0) {
			return;
		}
		sLocalCollisions.clear();
		iCollisionCount = 0;
	}
};

class CAction {
public:
	EActionType eActionType;
	SXY sXY;
	UINT iTime; // milliseconds
	bool bCancelable;
	bool bRepeat;

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
		cDefaultAction.bRepeat = false;
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
		if (cActions.size() != 0) {
			cActions.pop_front();
		}
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
	SXY sStartXY = { 0.0, 0.0 };
	SXY sXY = { 0.0, 0.0 };
	WH sWH;
	float fSpeedPerSeconds = 10.0;
	float fMagnification = 1.0;
	char szBitmapPath[MAX_PATH];
	SAniInfo arAniInfos[EActionType::Count];
	CActionList cActionList;
	CActionList cActionListPattern;
	HDC hBitmapDC;
	UINT _iAniIndex = 0;
	CSpriteInfo _cCurSpriteInfo;
	bool bControlled = false;

private:
	bool _bPatternPlaying = false;
	// direction
	float _fDirectionRadian = 0.0;
	V2 _v2NormalDirection = {};
	float _fDirectionWithCosRight = 0.0;
	// ani
	time_t _iAniTime = 0;
	// pos
	time_t _iWaitTimeOnPosition = 0;

public:
	CUnit();
	~CUnit();
	CUnit& operator=(const CUnit& r);
	void Init(HDC hdc);
	void Update(float fDeltaTime);
	void Render(HDC hdc);
	void Reset();
	void ClearAni();
	void Clear();
	void LoadUnit(CFilePath *cFilePath);
	void SetName(const char *name);
	void LoadUnitBitmap(const char *filePath);
	void ClearUnitBitmap();
	void LoadAniFile(EActionType eActionType, const char *filePath);
	void NextAction();
	RECT GetCollision() const;
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
