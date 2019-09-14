#include "stdafx.h"
#include "07-Tool-PhysicsTester.h"
#include "CPhysicsTester.h"
#include "lib.h"

HINSTANCE g_hInst;
HWND g_hDlg;
HWND g_hCanvas;
CPhysicsTester g_cPhysicsTester;

INT_PTR CALLBACK DlgProc(HWND, UINT, WPARAM, LPARAM);

int APIENTRY wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPWSTR lpCmdLine, _In_ int nCmdShow) {
	g_hInst = hInstance;
	g_hDlg = CreateDialog(g_hInst, MAKEINTRESOURCE(IDD_DIALOG1), nullptr, DlgProc);
	if (!g_hDlg) return 0;

	g_hCanvas = GetDlgItem(g_hDlg, IDC_PIC1);
	g_cPhysicsTester.Init(g_hDlg, g_hCanvas, 1000 / 60, { 800, 600 }, EWindowMode::EWindowMode_None);
	g_cPhysicsTester.PlayStop(true);

	MSG msg;
	while (true) {
		if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)) {
			if (msg.message == WM_QUIT) break;
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		if (g_cPhysicsTester.UpdateFrame()) {
			g_cPhysicsTester.UpdateLogic();
			g_cPhysicsTester.UpdateController();
			//if (GetFocus() == g_hCanvas) {
			//	g_cPhysicsTester.UpdateController();
			//}
			g_cPhysicsTester.UpdateRender();
		};
	}

	return (int)msg.wParam;
}
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
	case WM_LBUTTONDOWN: {
		g_cPhysicsTester.AddForce({-1.0, -1.0});
		break;
	}
	case WM_RBUTTONDOWN: {
		g_cPhysicsTester.AddForce({ 1.0, -1.0 });
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