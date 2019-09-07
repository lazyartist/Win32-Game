#include "stdafx.h"
#include "06-Tool-StageCreator.h"
#include "CStageCreator.h"

HINSTANCE hInst;

BOOL                InitInstance(HINSTANCE, int);
INT_PTR CALLBACK    DlgProc(HWND, UINT, WPARAM, LPARAM);

CStageCreator g_cStageCreator;

int APIENTRY wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPWSTR    lpCmdLine, _In_ int       nCmdShow) {
	if (!InitInstance(hInstance, nCmdShow)) {
		return FALSE;
	}

	HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_MY06TOOLSTAGECREATOR));

	MSG msg;

	while (GetMessage(&msg, nullptr, 0, 0)) {
		if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg)) {
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	return (int)msg.wParam;
}

BOOL InitInstance(HINSTANCE hInstance, int nCmdShow) {
	hInst = hInstance;

	HWND hWnd = CreateDialog(hInst, MAKEINTRESOURCE(IDD_DIALOG1), nullptr, DlgProc);

	if (!hWnd) {
		return FALSE;
	}

	return TRUE;
}

// 정보 대화 상자의 메시지 처리기입니다.
INT_PTR CALLBACK DlgProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam) {
	switch (message) {
	case WM_INITDIALOG: {
		return (INT_PTR)TRUE;
	}
	case WM_COMMAND: {
		if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL) {
			EndDialog(hDlg, LOWORD(wParam));
			return (INT_PTR)TRUE;
		}
		break;
	}
	case WM_CLOSE: {
		PostQuitMessage(0);
		return (INT_PTR)TRUE;
		break;
	}
	}

	return (INT_PTR)FALSE;
}
