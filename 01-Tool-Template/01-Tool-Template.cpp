﻿#include "stdafx.h"
#include "01-Tool-Template.h"
#include <commdlg.h> // GetOpenFileName()
#include <iostream> // sprintf_s()

#define MAX_LOADSTRING 100

POINT g_whMainWndSize = { 800, 600 };
POINT g_whBottomWndSize = { 800, 300 };
POINT g_whRightWndSize = { 0, 0 }; // 리소스에 의해 결정

// 전역 변수:
HINSTANCE g_hInst;                                // 현재 인스턴스입니다.

HWND g_hMainWnd;                                  // 메인 윈도우
HWND g_hRightWnd;                                 // 오른쪽 윈도우
HWND g_hBottomWnd;                                // 하단 윈도우

WCHAR g_szMainWndTitle[] = L"Main";               // 제목 표시줄 텍스트
WCHAR g_szBottomWndTitle[] = L"Bottom";           // 제목 표시줄 텍스트

WCHAR g_szMainWndClass[] = L"MainWnd";            // 메인창 클래스 이름
WCHAR g_szBottomWndClass[] = L"BottomWnd";        // 하단창 클래스 이름

const UINT g_nRightDlgId = IDD_DIALOG1;           // 우측 다이얼로그 리소스 ID
const UINT g_nMenuId = IDC_MY01TOOL;			  // 메뉴 ID
const UINT g_nIconId = IDI_MY01TOOL;			  // 아이콘 ID
const UINT g_nSmallIconId = IDI_SMALL;			  // 작은 아이콘 ID
const UINT g_nMenuId_OpenFile = ID_32771;		  // 파일 열기 메뉴 ID
const UINT g_nMenuId_SaveFile = ID_32772;		  // 파일 저장 메뉴 ID

ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);

LRESULT CALLBACK    MainWndProc(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK    BottomWndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    RightDlgProc(HWND, UINT, WPARAM, LPARAM);

void UpdateSubWndPosition();
void SetWindowPositionToCenter(HWND hWnd);

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPWSTR    lpCmdLine,
	_In_ int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

	MyRegisterClass(hInstance);

	// 응용 프로그램 초기화를 수행합니다:
	if (!InitInstance(hInstance, nCmdShow))
	{
		return FALSE;
	}

	MSG msg;

	// 기본 메시지 루프입니다:
	while (GetMessage(&msg, nullptr, 0, 0))
	{
		if (!TranslateAccelerator(msg.hwnd, nullptr, &msg))
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

	// 메인 윈도우 등록
	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = MainWndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance;
	wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(g_nIconId));
	wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wcex.lpszMenuName = MAKEINTRESOURCEW(g_nMenuId);
	wcex.lpszClassName = g_szMainWndClass;
	wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(g_nSmallIconId));
	RegisterClassExW(&wcex);

	// 하단 윈도우 등록
	wcex.lpfnWndProc = BottomWndProc;
	wcex.lpszMenuName = nullptr;
	wcex.lpszClassName = g_szBottomWndClass;
	return RegisterClassExW(&wcex);
}

BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
	g_hInst = hInstance; // 인스턴스 핸들을 전역 변수에 저장합니다.

	// 메인 윈도우
	RECT clientRect = { 0, 0, g_whMainWndSize.x - 1, g_whMainWndSize.y - 1 };
	AdjustWindowRect(&clientRect, WS_OVERLAPPEDWINDOW, true);

	HWND hWnd = CreateWindowW(g_szMainWndClass, g_szMainWndTitle, (WS_OVERLAPPEDWINDOW & ~WS_MAXIMIZEBOX/*최대화 버튼 비활성화*/),
		CW_USEDEFAULT, 0, clientRect.right - clientRect.left, clientRect.bottom - clientRect.top, nullptr, nullptr, hInstance, nullptr);

	g_hMainWnd = hWnd;

	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);

	// 하단 윈도우
	clientRect = { 0, 0, g_whBottomWndSize.x - 1, g_whBottomWndSize.y - 1 };
	AdjustWindowRect(&clientRect, WS_OVERLAPPEDWINDOW, true);

	g_hBottomWnd = CreateWindowW(g_szBottomWndClass, g_szBottomWndTitle, (WS_POPUP|WS_CAPTION|WS_THICKFRAME|WS_VISIBLE),
		0, 0, clientRect.right - clientRect.left, clientRect.bottom - clientRect.top, hWnd, nullptr, hInstance, nullptr);

	ShowWindow(g_hBottomWnd, nCmdShow);
	UpdateWindow(g_hBottomWnd);

	// 우측 다이얼로그
	g_hRightWnd = CreateDialog(g_hInst, MAKEINTRESOURCE(g_nRightDlgId), hWnd, RightDlgProc);

	if (!hWnd)
	{
		return FALSE;
	}

	SetWindowPositionToCenter(hWnd);

	return TRUE;
}

LRESULT CALLBACK MainWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
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
		case g_nMenuId_OpenFile: // 메뉴 - 열기
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

		case g_nMenuId_SaveFile: // 메뉴 - 저장
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

INT_PTR CALLBACK RightDlgProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
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