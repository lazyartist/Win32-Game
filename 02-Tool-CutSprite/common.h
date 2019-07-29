#pragma once

#include <iostream> // sprintf_s()
#include "Windows.h"
//#include <math.h>
//#include <vector>

using namespace std;

// 함수 포인터
typedef LRESULT(CALLBACK *WndProc)(HWND, UINT, WPARAM, LPARAM); // 윈도우 프로시저
typedef INT_PTR(CALLBACK *DlgProc)(HWND, UINT, WPARAM, LPARAM); // 다이얼로그 프로시저

typedef void(*CallbackFunc)(); // 이벤트 콜백

// ===== define =====
#define nMax_Boxes 999
#define szMax_Boxes 4
#define szMax_Pos 6
#define szMax_PosLine 99
#define nFrameRate 60
#define nPivotHalfSize 5
#define nMax_SpriteCollision 1
#define nMax_SpriteCoordinateCount (4/*sprite*/ + 2/*pivot*/ + (nMax_SpriteCollision * 4))
#define nMax_SpriteCoordinateByteSize (sizeof(int) * nMax_SpriteCoordinateCount)
//
//#define Max_Student_Id 6 + 1
//#define Max_Student_Name 10 + 1
//#define Max_Student_Info_Line (Max_Student_Id + 1 + Max_Student_Name)
//
//#define Max_Score_Text 9 + 1
//#define Max_Score_Info_Line (Max_Student_Id + 1 + Max_Score_Text + 1 + Max_Score_Text + 1 + Max_Score_Text)
//
//#define Max_Semester 4
//
//#define Str_ScoreFile_Path_Format "scores/%s_%d.txt"

// ===== define ===== end


// ===== enum =====
enum MouseModeType
{
	Sprite, TransparentColor, Pivot, Collision
};
// ===== enum =====  end


// ===== struct =====
typedef struct _XY {
	INT x;
	INT y;
} XY;

typedef struct _WH {
	UINT w;
	UINT h;
} WH;

typedef struct _BitmapViewInfo {
	float Magnification = 2.0;
	//float Magnification = 60.0;
	//bool IsTransparentColorPickMode;
	bool IsMoveMode;
	COLORREF TransparentColor;
} BitmapViewInfo;

typedef struct _SpriteInfo {
	INT Coordinates[nMax_SpriteCoordinateCount];
	RECT Rect;
	XY Pivot;
	RECT Collisions[nMax_SpriteCollision];
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

		for (size_t j = 0; j < nMax_SpriteCollision; j++)
		{
			RECT collision = { Coordinates[i++] ,Coordinates[i++] ,Coordinates[i++] ,Coordinates[i++] };
			Collisions[j] = collision;
		}
	}
} SpriteInfo;
// ===== struct ===== end


// ===== function =====
inline WH ScreenRectToWH(RECT rect) {
	int w = rect.right - rect.left;
	int h = rect.bottom - rect.top;
	WH wh = { w, h };

	return wh;
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
// ===== operation overloading ===== end


// ===== log =====
// for string
inline void _dlog(UINT count, const char *args, ...) {
	va_list ap; // 가변인자 메모리 주소
	va_start(ap, args); // 가변인자 메모리 주소 설정

	OutputDebugString(args); // 변수가 있는 것은 그냥 사용
	for (size_t i = 0; i < count - 1; i++)
	{
		OutputDebugString(", ");

		char *str = va_arg(ap, char *); // 가변인자는 va_list에서 자료형 크기만큼 이동된 곳의 값을 사용
		OutputDebugString(str);

	}
	OutputDebugString("\n");

	va_end(ap);
};

// for int
inline void _dlog(const char *title, UINT count, int args, ...) {
	va_list ap; // 가변인자 메모리 주소
	va_start(ap, args); // 가변인자 메모리 주소 설정

	// 제목 출력
	if (title[0] != '\0') {
		OutputDebugString(title); // 변수가 있는 것은 그냥 사용
		OutputDebugString(" : "); // 변수가 있는 것은 그냥 사용
	}

	// 첫번째 인자 출력
	{
		char buffer[11];
		sprintf_s(buffer, 11, "%d", args);
		OutputDebugString(buffer);
	}

	// 가변인자 출력
	for (size_t i = 0; i < count - 1; i++)
	{
		// char, short, int는 int 타입 크기로 이동
		// 가변인자는 va_list에서 자료형 크기만큼 이동된 곳의 값을 사용
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
	va_list ap; // 가변인자 메모리 주소
	va_start(ap, args); // 가변인자 메모리 주소 설정

	// 제목 출력
	OutputDebugString(title); // 변수가 있는 것은 그냥 사용
	OutputDebugString(" : ");

	// 첫번째 인자 출력
	{
		char buffer[11];
		sprintf_s(buffer, 11, "%f", args);
		OutputDebugString(buffer);
	}

	// 가변인자 출력
	for (size_t i = 0; i < count - 1; i++)
	{
		// float는 double 타입 크기로 이동
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