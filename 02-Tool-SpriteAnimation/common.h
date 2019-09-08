#pragma once

// �Լ� ������
typedef LRESULT(CALLBACK *WndProc)(HWND, UINT, WPARAM, LPARAM); // ������ ���ν���
typedef INT_PTR(CALLBACK *DlgProc)(HWND, UINT, WPARAM, LPARAM); // ���̾�α� ���ν���

typedef void(*CallbackFunc)(); // �̺�Ʈ �ݹ�

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