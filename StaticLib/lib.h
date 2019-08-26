#pragma once

#include <vector>
#include <list>
#include "windows.h"
//#include "windowsx.h"
#include "function.h"

// ===== define =====
#define nMax_SpriteCount 999
#define szMax_SpriteCount 3 + 1
#define szMax_Pos 6
#define szMax_PosLine 99
#define nFrameRate 60 // ����� �ֻ����� 60hz�ε� 60���� ���̸� �� �ε巴�� ���δ�. ���ϱ�?
#define nPivotHalfSize 5
#define nMax_SpriteCollision 4
#define nMax_SpriteCoordinateCount (4/*sprite*/ + 2/*pivot*/)
#define nMax_SpriteCoordinateByteSize (sizeof(int) * nMax_SpriteCoordinateCount)
#define nMax_RectPos 4

#define szMax_XY 30 + 1
#define szMax_UnitStateLine 99
#define szMax_UnitName 99

#define nMax_UnitStateCount 999
#define szMax_UnitStateCount 3 + 1

#define NoSelectedIndex -1

using namespace std;

void RemoveCarriageReturn(char *sz);
const char * GetFileNameByFullPath(const char *path);

// ===== enum =====
enum WindowMode{
	None, Window, FullScreen
};
enum UnitStateType {
	Idle, Walk,
	Count
};
// ===== enum ===== end

// ===== struct =====
typedef struct _XY {
	INT x;
	INT y;
} XY;

typedef struct _fXY {
	FLOAT x;
	FLOAT y;

	float distance() {
		return sqrt((x) * (x) + (y) * (y));
	}
	float distance(_fXY xy) {
		return sqrt((xy.x - x) * (xy.x - x) + (xy.y - y) * (xy.y - y));
	}
} fXY;

typedef struct _WH {
	UINT w;
	UINT h;
} WH;

typedef struct _fWH {
	FLOAT w;
	FLOAT h;
} fWH;
// ===== struct ===== end

// ===== class ===== 
typedef class _SpriteInfo {
public:
	UINT Time = 0;
	INT Coordinates[nMax_SpriteCoordinateCount];
	RECT Rect;
	XY Pivot;
	//RECT Collisions[nMax_SpriteCollision];
	vector<RECT> Collisions;
	UINT CollisionCount;

	void SetCoordinates(INT coordinates[], UINT count) {
		memcpy(Coordinates, coordinates, count);

		int i = 0;
		Rect.left = Coordinates[i++];
		Rect.top = Coordinates[i++];
		Rect.right = Coordinates[i++];
		Rect.bottom = Coordinates[i++];
		Pivot.x = Coordinates[i++];
		Pivot.y = Coordinates[i++];
	}
	void SetPivot(INT x, INT y) {
		int i = 0;
		Rect.left = Coordinates[i++];
		Rect.top = Coordinates[i++];
		Rect.right = Coordinates[i++];
		Rect.bottom = Coordinates[i++];
		Coordinates[4] = x;
		Coordinates[5] = y;
		Pivot.x = x;
		Pivot.y = y;
	}
	void ResetCollisionCount() {
		CollisionCount = 0;
	}
	void AddCollision(int collisionPos[], int count) {
		if (CollisionCount >= nMax_SpriteCollision) {
			return;
		}

		RECT collision = { collisionPos[0] , collisionPos[1] , collisionPos[2] , collisionPos[3] };
		//Collisions[CollisionCount++] = collision;

		++CollisionCount;
		Collisions.push_back(collision);
	}
	void RemoveCollision(int index) {
		if (index < 0 || index >= CollisionCount) {
			return;
		}

		//auto iter = Collisions.begin();
		vector<RECT>::iterator iter = Collisions.begin();
		Collisions.erase(iter + index);
		--CollisionCount;
	}
	void RemoveAllCollisions() {
		if (CollisionCount == 0) {
			return;
		}

		Collisions.clear();
		CollisionCount = 0;
	}
} SpriteInfo;

class UnitState {
public:
	UnitStateType UnitStateType;
	fXY XY;
	UINT Time; // milliseconds

	UnitState() {
		//
	}
};

class UnitStatePattern {
public:
	vector<UnitState> UnitStates;
	UINT UnitStateIndex;

	UnitStatePattern() {
		//
	}

	~UnitStatePattern() {
		//
	}

	UnitState GetCurUnitState() {
		return UnitStates[UnitStateIndex];
	}

	void UpUnitStateIndex() {
		++UnitStateIndex;

		if (UnitStateIndex == UnitStates.size()) {
			UnitStateIndex = 0;
		}
	}

	void AddUnitState(UnitState unitState)
	{
		UnitStates.push_back(unitState);
	}

	bool DeleteUnitState(UINT index)
	{
		if (index >= UnitStates.size()) return false;

		auto iter = UnitStates.begin();
		UnitStates.erase(iter + index);

		return true;
	}

	bool DeleteAllUnitState()
	{
		UnitStates.clear();

		return true;
	}

	bool UpUnitState(UINT index)
	{
		if (index == 0 || index >= UnitStates.size()) return false;

		UnitState unitStateTemp = UnitStates[index - 1];
		UnitStates[index - 1] = UnitStates[index];
		UnitStates[index] = unitStateTemp;

		return true;
	}

	bool DownUnitState(UINT index)
	{
		if (index < 0 || index >= UnitStates.size() - 1) return false;

		UnitState unitStateTemp = UnitStates[index + 1];
		UnitStates[index + 1] = UnitStates[index];
		UnitStates[index] = unitStateTemp;

		return true;
	}

	void LoadUnitStatePatternFile(const char *filePath) {
		FILE *file = nullptr;
		file = _fsopen(filePath, "rt", _SH_DENYNO);

		if (file == nullptr) return;

		char szItemCount[szMax_SpriteCount] = {};
		fgets(szItemCount, szMax_SpriteCount, file);

		// item count
		int itemCount = atoi(szItemCount);

		UnitStates.clear();
		UnitStates.reserve(itemCount);

		// ===== List�� ������ �߰� =====
		char itemLine[szMax_UnitStateLine] = {};
		char *token;
		char *nextToken;

		char itemText[szMax_XY] = {};
		for (size_t i = 0; i < itemCount; i++)
		{
			memset(itemLine, 0, szMax_UnitStateLine);
			fgets(itemLine, szMax_UnitStateLine, file);

			// \n�� �ٹٲ��� �����ϴ� �����̹Ƿ� ���� ���ڸ� ��� ���� �����Ѵ�.
			itemLine[strcspn(itemLine, "\n")] = 0; // strcspn()���� "\n"�� ��ġ�� ã�� �� ��ġ�� 0�� �־��ش�.

			if (strnlen_s(itemLine, szMax_PosLine) == 0) continue;

			UnitState unitState;
			{
				nextToken = itemLine;

				token = strtok_s(nullptr, "\t", &nextToken);
				unitState.UnitStateType = (UnitStateType)atoi(token);

				token = strtok_s(nullptr, "\t", &nextToken);
				unitState.XY.x = atof(token);
				token = strtok_s(nullptr, "\t", &nextToken);
				unitState.XY.y = atof(token);
				token = strtok_s(nullptr, "\t", &nextToken);
				unitState.Time = atoi(token);
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
		int itemCount = UnitStates.size();
		char szItemCount[szMax_UnitStateCount] = {};
		sprintf_s<szMax_UnitStateCount>(szItemCount, "%d\n", itemCount);
		fputs(szItemCount, file);

		auto iter = UnitStates.begin();
		while (iter != UnitStates.end())
		{
			fprintf_s(file, "%d\t%f\t%f\t%d\n", iter->UnitStateType, iter->XY.x, iter->XY.y, iter->Time);

			++iter;
		}

		fclose(file);
	}
};

struct AniInfo {
	AniInfo() {

	}

	~AniInfo() {
		//if (hBitmapDC) {
			//DeleteObject(hBitmapDC);
		//}
	}
	char FilePath[MAX_PATH];
	char FileTitle[MAX_PATH];
	//char BitmapPath[MAX_PATH];
	vector<SpriteInfo> SpriteInfos;
	//HDC hBitmapDC;
};

class Unit {
public:
	char Name[szMax_UnitName];
	fXY XY;
	WH WH;
	UnitStateType CurUnitStateType;
	char BitmapPath[MAX_PATH];
	AniInfo AniInfos[UnitStateType::Count];
	//vector<AniFilePath> AniInfos;
	UnitStatePattern UnitStatePattern;
	float SpeedPerSeconds = 1;
	//vector<SpriteInfo> SpriteInfosByUnitStateType[sizeof(UnitStateType)];
	HDC hBitmapDC;
	bool _isPlaying = false;
	bool _isPatternPlaying = false;
	time_t _aniTime = 0;
	UINT _aniIndex = 0;
	SpriteInfo _curSpriteInfo;

	Unit() {
		for (size_t i = 0; i < UnitStateType::Count; i++)
		{
			AniInfos[i].FilePath[0] = 0;
			AniInfos[i].FileTitle[0] = 0;
		}
		//AniInfos
	}

	void Init(HDC hdc) {
		hBitmapDC = CreateCompatibleDC(hdc);
	}
	
	void Update() {
		if (_isPlaying) {
			time_t time = GetTickCount();

			if (time - _aniTime < _curSpriteInfo.Time) return;

			_aniTime = time;
			++_aniIndex;

			vector<SpriteInfo> &spriteInfos = AniInfos[(int)CurUnitStateType].SpriteInfos;
			if (_aniIndex >= spriteInfos.size()) {
				_aniIndex = 0;
			}

			_curSpriteInfo = spriteInfos[_aniIndex];
		}
	}
	
	void Render(HDC hdc) {
		if (!_isPlaying) return;

		// test
		fWH g_whBottomWndSize = {800, 600};

		//vector<SpriteInfo> &spriteInfos = AniInfos[(int)CurUnitStateType].SpriteInfos;
		//SpriteInfo _curSpriteInfo = spriteInfos[_aniIndex];

		// ȭ�� ���߾� ��ǥ
		fXY center = { g_whBottomWndSize.w / 2, g_whBottomWndSize.h / 2 };

		// ���õ� ��������Ʈ�� ũ��
		fWH spriteSize = { _curSpriteInfo.Rect.right - _curSpriteInfo.Rect.left , _curSpriteInfo.Rect.bottom - _curSpriteInfo.Rect.top };
		UINT w = _curSpriteInfo.Rect.right - _curSpriteInfo.Rect.left;
		UINT h = _curSpriteInfo.Rect.bottom - _curSpriteInfo.Rect.top;
		TransparentBlt(hdc,
			center.x - _curSpriteInfo.Pivot.x,
			//center.x - _curSpriteInfo.Pivot.x * fMagnification,
			center.y - _curSpriteInfo.Pivot.y,
			//center.y - _curSpriteInfo.Pivot.y * fMagnification,

			spriteSize.w,
			//spriteSize.w * fMagnification,
			spriteSize.h,
			//spriteSize.h * fMagnification,

			hBitmapDC,
			_curSpriteInfo.Rect.left,
			_curSpriteInfo.Rect.top,
			spriteSize.w,
			spriteSize.h,

			RGB(255, 0, 0));
			//g_BitmapViewInfo.TransparentColor);
	}

	void Play(UnitStateType unitStateType) {
		CurUnitStateType = unitStateType;
		_isPlaying = true;
		_aniIndex = 0;
		_aniTime = GetTickCount();

		vector<SpriteInfo> &spriteInfos = AniInfos[(int)CurUnitStateType].SpriteInfos;
		_curSpriteInfo = spriteInfos[0];
	}

	void Stop() {
		//CurUnitStateType = unitStateType;
		_isPlaying = false;
		//_aniIndex = 0;
	}

	void SetName(const char *name) {
		strcpy_s(Name, szMax_UnitName, name);
	}

	void LoadUnitBitmap(const char *filePath) {
		strcpy_s(BitmapPath, MAX_PATH, filePath);

		HBITMAP hBitmap = (HBITMAP)LoadImage(nullptr, filePath, IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
		SelectObject(hBitmapDC, hBitmap); // HBITMAP�� HDC�� ����Ǹ� �ٽ� ����� �� ���� ������ ������ ���� HDC�� �־�д�.
		DeleteObject(hBitmap);
	}

	void LoadAniFile(UnitStateType unitStateType, const char *filePath) {
		FILE *file = nullptr;
		file = _fsopen(filePath, "rt", _SH_DENYNO);

		if (file == nullptr) return;

		AniInfo &aniFile = AniInfos[(int)unitStateType];
		strcpy_s(aniFile.FilePath, MAX_PATH, filePath);
		strcpy_s(aniFile.FileTitle, MAX_PATH, GetFileNameByFullPath(filePath));

		aniFile.SpriteInfos.clear();

		char temp[MAX_PATH];
		fgets(temp, MAX_PATH, file); // bitmap path ������� �����Ƿ� �ӽ� ������ �����´�.(�� �� �ǳʶٴ� ȿ��)
		//RemoveCarriageReturn(aniFile.BitmapPath);

		char szItemCount[szMax_SpriteCount] = {};
		fgets(szItemCount, szMax_SpriteCount, file);

		// item count
		int itemCount = atoi(szItemCount);

		aniFile.SpriteInfos.reserve(itemCount);

		// ===== List�� ������ �߰� =====
		char itemLine[szMax_PosLine] = {};
		char *token;
		char *nextToken;

		char itemText[szMax_Pos] = {};
		for (size_t i = 0; i < itemCount; i++)
		{
			memset(itemLine, 0, szMax_PosLine);
			fgets(itemLine, szMax_PosLine, file);

			// \n�� �ٹٲ��� �����ϴ� �����̹Ƿ� ���� ���ڸ� ��� ���� �����Ѵ�.
			itemLine[strcspn(itemLine, "\n")] = 0; // strcspn()���� "\n"�� ��ġ�� ã�� �� ��ġ�� 0�� �־��ش�.

			if (strnlen_s(itemLine, szMax_PosLine) == 0) continue;

			SpriteInfo spriteInfo;
			{
				nextToken = itemLine;

				token = strtok_s(nullptr, "\t", &nextToken);
				spriteInfo.Time = atoi(token);

				// rect, pivot
				int coordinates[nMax_SpriteCoordinateCount] = {  };
				for (size_t j = 0; j < nMax_SpriteCoordinateCount; j++)
				{
					token = strtok_s(nullptr, "\t", &nextToken);
					coordinates[j] = atoi(token);
				}
				spriteInfo.SetCoordinates(coordinates, sizeof(INT) * nMax_SpriteCoordinateCount);

				// collision
				token = strtok_s(nullptr, "\t", &nextToken);
				INT collisionCount = atoi(token);
				spriteInfo.ResetCollisionCount();

				for (size_t j = 0; j < collisionCount; j++)
				{
					int collisionPos[nMax_RectPos];
					for (size_t k = 0; k < nMax_RectPos; k++)
					{
						token = strtok_s(nullptr, "\t", &nextToken);
						collisionPos[k] = atoi(token);
					}

					spriteInfo.AddCollision(collisionPos, nMax_RectPos);
				}

			}

			aniFile.SpriteInfos.push_back(spriteInfo);
		}

		fclose(file);

		//strcpy_s(g_szAniFilePath, MAX_PATH, filePath);
		//SetDlgItemText(g_hRightWnd, IDC_EDIT5, GetFileNameByFullPath(g_szAniFilePath));
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

inline fXY operator-(fXY xy1, fXY xy2) {
	xy1.x -= xy2.x;
	xy1.y -= xy2.y;

	return xy1;
}

inline bool operator==(fXY xy1, fXY xy2) {
	return (xy1.x == xy2.x) && (xy1.y == xy2.y);
}

inline bool sameXY(fXY xy1, fXY xy2) {
	return (abs(xy1.x - xy2.x) < 0.1) && (abs(xy1.x - xy2.x) < 0.1);
	//return (abs(xy1.x - xy2.x) < 0.01) && (abs(xy1.x - xy2.x) < 0.01);
	//return (abs(xy1.x - xy2.x) < 0.001) && (abs(xy1.x - xy2.x) < 0.001);
}
// ===== operation overloading ===== end


// ===== log =====
// for string
inline void _dlog(UINT count, const char *args, ...) {
	va_list ap; // �������� �޸� �ּ�
	va_start(ap, args); // �������� �޸� �ּ� ����

	OutputDebugString(args); // ������ �ִ� ���� �׳� ���
	for (size_t i = 0; i < count - 1; i++)
	{
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
	for (size_t i = 0; i < count - 1; i++)
	{
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
		char buffer[11];
		sprintf_s(buffer, 11, "%f", args);
		OutputDebugString(buffer);
	}

	// �������� ���
	for (size_t i = 0; i < count - 1; i++)
	{
		// float�� double Ÿ�� ũ��� �̵�
		// https://stackoverflow.com/questions/11270588/variadic-function-va-arg-doesnt-work-with-float
		float arg = va_arg(ap, double);

		OutputDebugString(", ");

		char buffer[11];
		sprintf_s(buffer, 11, "%f", arg);
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

inline void dlog(RECT rect) {
	_dlog("", 4, rect.left, rect.top, rect.right, rect.bottom);
};

inline void dlog(const char *title, RECT rect) {
	_dlog(title, 4, rect.left, rect.top, rect.right, rect.bottom);
};
// ===== log ===== end