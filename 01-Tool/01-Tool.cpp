// 01-Tool.cpp : 응용 프로그램에 대한 진입점을 정의합니다.
//

#include "stdafx.h"
#include "01-Tool.h"
#include <commdlg.h> // GetOpenFileName
#include <iostream> // sprintf_s

#define MAX_LOADSTRING 100

POINT g_whMainWndSize = { 800, 600 };
POINT g_whRightWndSize = { 0, 0 };
POINT g_whBottomWndSize = { 800, 300 };

// 전역 변수:
HINSTANCE g_hInst;                                // 현재 인스턴스입니다.
HWND g_hMainWnd;                                // 메인 윈도우
HWND g_hRightWnd;                                // 오른쪽 윈도우
HWND g_hBottomWnd;                                // 하단 윈도우

WCHAR szTitle[MAX_LOADSTRING];                  // 제목 표시줄 텍스트입니다.
WCHAR szWindowClass[MAX_LOADSTRING];            // 기본 창 클래스 이름입니다.

// 이 코드 모듈에 포함된 함수의 선언을 전달합니다:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK    BottomWndProc(HWND, UINT, WPARAM, LPARAM);

INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    RightWndProc(HWND, UINT, WPARAM, LPARAM);

void UpdateSubWndPosition();
void SetWindowPositionToCenter(HWND hWnd);

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPWSTR    lpCmdLine,
	_In_ int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

	// TODO: 여기에 코드를 입력합니다.

	// 전역 문자열을 초기화합니다.
	LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadStringW(hInstance, IDC_MY01TOOL, szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance);

	// 응용 프로그램 초기화를 수행합니다:
	if (!InitInstance(hInstance, nCmdShow))
	{
		return FALSE;
	}

	HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_MY01TOOL));

	MSG msg;

	// 기본 메시지 루프입니다:
	while (GetMessage(&msg, nullptr, 0, 0))
	{
		if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	return (int)msg.wParam;
}


ATOM MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEXW wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = WndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance;
	wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_MY01TOOL));
	wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wcex.lpszMenuName = MAKEINTRESOURCEW(IDC_MY01TOOL);
	wcex.lpszClassName = szWindowClass;
	wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

	RegisterClassExW(&wcex);

	wcex.lpfnWndProc = BottomWndProc;
	wcex.lpszMenuName = nullptr;
	wcex.lpszClassName = L"BottomWnd";

	return RegisterClassExW(&wcex);
}

BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
	g_hInst = hInstance; // 인스턴스 핸들을 전역 변수에 저장합니다.

	// 메인 윈도우
	RECT clientRect = { 0, 0, g_whMainWndSize.x - 1, g_whMainWndSize.y - 1 };
	AdjustWindowRect(&clientRect, WS_OVERLAPPEDWINDOW, true);

	HWND hWnd = CreateWindowW(szWindowClass, szTitle, (WS_OVERLAPPEDWINDOW & ~WS_MAXIMIZEBOX/*최대화 버튼 비활성화*/),
		CW_USEDEFAULT, 0, clientRect.right - clientRect.left, clientRect.bottom - clientRect.top, nullptr, nullptr, hInstance, nullptr);

	g_hMainWnd = hWnd;

	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);

	// 하단 윈도우
	clientRect = { 0, 0, g_whBottomWndSize.x - 1, g_whBottomWndSize.y - 1 };
	AdjustWindowRect(&clientRect, WS_OVERLAPPEDWINDOW, true);

	g_hBottomWnd = CreateWindowW(L"BottomWnd", L"Bottom", (WS_POPUP|WS_CAPTION|WS_THICKFRAME|WS_VISIBLE),
		0, 0, clientRect.right - clientRect.left, clientRect.bottom - clientRect.top, hWnd, nullptr, hInstance, nullptr);

	ShowWindow(g_hBottomWnd, nCmdShow);
	UpdateWindow(g_hBottomWnd);

	// 우측 다이얼로그
	g_hRightWnd = CreateDialog(g_hInst, MAKEINTRESOURCE(IDD_DIALOG1), hWnd, RightWndProc);

	if (!hWnd)
	{
		return FALSE;
	}

	SetWindowPositionToCenter(hWnd);

	return TRUE;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_CREATE:
	{
		
	}
	break;
	case WM_COMMAND:
	{
		int wmId = LOWORD(wParam);
		// 메뉴 선택을 구문 분석합니다:
		switch (wmId)
		{
		case IDM_ABOUT:
			DialogBox(g_hInst, MAKEINTRESOURCE(IDD_DIALOG2), hWnd, About);
			break;

		case ID_32771: // 메뉴 - 열기
		{
			// static 또는 전역변수로 선언하지 않으면 다이얼로그가 열리지 않음
			static char lpstrFile[MAX_PATH];
			// static 또는 전역변수로 선언하지 않아도 다이얼로그가 열림(강의에서는 필요하다고 함)
			char lpstrFileTitle[MAX_PATH];

			OPENFILENAME ofn = { 0, };
			ofn.lStructSize = sizeof(OPENFILENAME);
			ofn.hwndOwner = hWnd;
			ofn.lpstrFilter = "Every File(*.*)\0*.*\0";
			ofn.lpstrFile = lpstrFile;
			ofn.lpstrFileTitle = lpstrFileTitle;
			ofn.nMaxFile = MAX_PATH;
			ofn.nMaxFileTitle = MAX_PATH;
			ofn.lpstrTitle = "title";
			ofn.lpstrInitialDir = "C:\\";

			if (GetOpenFileName(&ofn)) {
				char szResult[MAX_PATH];
				sprintf_s(szResult, "Path: %s\n%s\n%d", ofn.lpstrFile, ofn.lpstrFileTitle, ofn.nFilterIndex/*1부터 시작*/);
				MessageBox(hWnd, szResult, NULL, MB_OK);
			};
		}
		break;

		case ID_32772: // 메뉴 - 저장
		{
			// static 또는 전역변수로 선언하지 않으면 다이얼로그가 열리지 않음
			static char lpstrFile[MAX_PATH];
			// static 또는 전역변수로 선언하지 않아도 다이얼로그가 열림(강의에서는 필요하다고 함)
			char lpstrFileTitle[MAX_PATH];

			OPENFILENAME ofn = { 0, };
			ofn.lStructSize = sizeof(OPENFILENAME);
			ofn.hwndOwner = hWnd;
			ofn.lpstrFilter = "Every File(*.*)\0*.*\0";
			ofn.lpstrFile = lpstrFile;
			ofn.lpstrFileTitle = lpstrFileTitle;
			ofn.nMaxFile = MAX_PATH;
			ofn.nMaxFileTitle = MAX_PATH;
			ofn.lpstrTitle = "title";
			ofn.lpstrInitialDir = "C:\\";

			if (GetSaveFileName(&ofn)) {
				char szResult[MAX_PATH];
				sprintf_s(szResult, "Path: %s\n%s", ofn.lpstrFile, ofn.lpstrFileTitle);
				MessageBox(hWnd, szResult, NULL, MB_OK);
			};
		}
		break;

		case IDM_EXIT:
			DestroyWindow(hWnd);
			break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
	}
	break;
	case WM_PAINT:
	{
		PAINTSTRUCT ps;
		HDC hdc = BeginPaint(hWnd, &ps);
		// TODO: 여기에 hdc를 사용하는 그리기 코드를 추가합니다...
		EndPaint(hWnd, &ps);
	}
	break;
	case WM_MOVE:
	{
		//POINTS xy = MAKEPOINTS(lParam);

		// 서브윈도우가 항상 메인윈도우 오른쪽과 하단에 있게 함
		UpdateSubWndPosition();
	}
	break;
	case WM_SIZE:
	{
		// 메인윈도우 크기조절 불가
		RECT rect;
		GetWindowRect(g_hMainWnd, &rect);
		MoveWindow(g_hMainWnd, rect.left, rect.top, g_whMainWndSize.x, g_whMainWndSize.y, true);
	}
	break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}


LRESULT CALLBACK BottomWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_CREATE:
	{
		SetWindowPositionToCenter(hWnd);
	}
	break;
	case WM_COMMAND:
	{
		int wmId = LOWORD(wParam);
		// 메뉴 선택을 구문 분석합니다:
		switch (wmId)
		{
		case IDM_EXIT:
			DestroyWindow(hWnd);
			break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
	}
	break;
	case WM_PAINT:
	{
		PAINTSTRUCT ps;
		HDC hdc = BeginPaint(hWnd, &ps);
		// TODO: 여기에 hdc를 사용하는 그리기 코드를 추가합니다...
		EndPaint(hWnd, &ps);
	}
	break;
	case WM_MOVE:
	{
		// 위치 조절 불가
		UpdateSubWndPosition();
	}
	case WM_SIZE:
	{
		// 메인윈도우 크기조절 불가
		RECT rect;
		GetWindowRect(g_hMainWnd, &rect);
		MoveWindow(g_hMainWnd, rect.left, rect.top, g_whMainWndSize.x, g_whMainWndSize.y, true);
	}
	break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);
	switch (message)
	{
	case WM_INITDIALOG:
		return (INT_PTR)TRUE;

	case WM_COMMAND:
		if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
		{
			EndDialog(hDlg, LOWORD(wParam));
			return (INT_PTR)TRUE;
		}
		break;
	}
	return (INT_PTR)FALSE;
}

INT_PTR CALLBACK RightWndProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);
	switch (message)
	{
	case WM_INITDIALOG:
	{
		RECT mainWndRect;
		GetWindowRect(g_hMainWnd, &mainWndRect);

		RECT dlgWndRect;
		GetClientRect(hDlg, &dlgWndRect);
		g_whRightWndSize.x = dlgWndRect.right - dlgWndRect.left;
		g_whRightWndSize.y = dlgWndRect.bottom - dlgWndRect.top;

		MoveWindow(hDlg, mainWndRect.right, mainWndRect.top, g_whRightWndSize.x, g_whRightWndSize.y, true);
	}
	return (INT_PTR)TRUE;

	case WM_COMMAND:
		if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
		{
			EndDialog(hDlg, LOWORD(wParam));
			return (INT_PTR)TRUE;
		}
		break;
	}
	return (INT_PTR)FALSE;
}

// 윈도우의 위치를 스크린 가운데로 옮김
void SetWindowPositionToCenter(HWND hWnd) {
	UpdateSubWndPosition();

	int screenX = GetSystemMetrics(SM_CXFULLSCREEN);
	int screenY = GetSystemMetrics(SM_CYFULLSCREEN);

	RECT rectMainWnd;
	RECT rectRightWnd;
	RECT rectBottomWnd;
	GetWindowRect(g_hMainWnd, &rectMainWnd);
	GetWindowRect(g_hRightWnd, &rectRightWnd);
	GetWindowRect(g_hBottomWnd, &rectBottomWnd);

	UINT clientW = rectRightWnd.right - rectMainWnd.left;
	UINT clientH = rectBottomWnd.bottom - rectMainWnd.top;

	MoveWindow(hWnd, (screenX / 2) - (clientW / 2), (screenY / 2) - (clientH / 2), clientW, clientH, false);

	UpdateSubWndPosition();
}

void UpdateSubWndPosition() {
	RECT rect;
	GetWindowRect(g_hMainWnd, &rect);
	MoveWindow(g_hRightWnd, rect.right + 2, rect.top, g_whRightWndSize.x, g_whRightWndSize.y, true);
	MoveWindow(g_hBottomWnd, rect.left, rect.bottom + 2, g_whBottomWndSize.x, g_whBottomWndSize.y, true);
}