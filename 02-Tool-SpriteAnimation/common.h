#pragma once

// 함수 포인터
typedef LRESULT(CALLBACK *WndProc)(HWND, UINT, WPARAM, LPARAM); // 윈도우 프로시저
typedef INT_PTR(CALLBACK *DlgProc)(HWND, UINT, WPARAM, LPARAM); // 다이얼로그 프로시저

typedef void(*CallbackFunc)(); // 이벤트 콜백

// ===== enum =====
enum MouseModeType
{
	Sprite, TransparentColor, Pivot, Collision
};
// ===== enum =====  end
// ===== struct =====
typedef struct _BitmapViewInfo {
	float Magnification = 2.0;
	//float fMagnification = 60.0;
	//bool IsTransparentColorPickMode;
	bool IsMoveMode;
	COLORREF TransparentColor;
} BitmapViewInfo;