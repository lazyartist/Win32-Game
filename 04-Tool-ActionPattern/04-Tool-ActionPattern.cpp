﻿#include <list> 
#include <vector> 
#include "stdafx.h"
#include <windowsx.h> // GET_X_LPARAM
#include "Commctrl.h"
#include <commdlg.h>
#include "04-Tool-ActionPattern.h"
#include "CActionPattern.h"

#define MAX_LOADSTRING 100

// 전역 변수:
HINSTANCE g_hInstance;                                // 현재 인스턴스입니다.
WCHAR szTitle[MAX_LOADSTRING];                  // 제목 표시줄 텍스트입니다.
WCHAR szWindowClass[MAX_LOADSTRING];            // 기본 창 클래스 이름입니다.

WH g_whClientSize = { 800, 600 };
char g_szAniFilePath[MAX_PATH];

HWND g_hWnd;
HWND g_hDlg;
CActionPattern g_cActionPattern;
HWND g_hActionList;

void SetWindowPositionToCenter(HWND hWnd);
void UpdateSubWndPosition();
void UpdateActions();

// 이 코드 모듈에 포함된 함수의 선언을 전달합니다:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
INT_PTR CALLBACK    DlgProc(HWND, UINT, WPARAM, LPARAM);

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPWSTR    lpCmdLine,
	_In_ int       nCmdShow) {
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

	// TODO: 여기에 코드를 입력합니다.

	// 전역 문자열을 초기화합니다.
	LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadStringW(hInstance, IDC_MY04TOOLACTIONPATTERN, szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance);

	// 응용 프로그램 초기화를 수행합니다:
	if (!InitInstance(hInstance, nCmdShow)) {
		return FALSE;
	}

	g_cActionPattern.Init(g_hWnd, g_hWnd, 1000 / 90, { g_whClientSize.w, g_whClientSize.h }, EWindowMode::EWindowMode_None);
	SetWindowPositionToCenter(g_hDlg);

	MSG msg;

	while (true) {
		if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)) {
			if (msg.message == WM_QUIT) break;
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		if (g_cActionPattern.UpdateFrame()) {
			g_cActionPattern.UpdateLogic();
			/*if (GetFocus() == g_hCanvas) {
				g_cStageCreator.UpdateController();
			}*/
			g_cActionPattern.UpdateRender();
		};
	}
	g_cActionPattern.Release();

	return (int)msg.wParam;
}

ATOM MyRegisterClass(HINSTANCE hInstance) {
	WNDCLASSEXW wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = WndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance;
	wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_MY04TOOLACTIONPATTERN));
	wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wcex.lpszMenuName = MAKEINTRESOURCEW(IDC_MY04TOOLACTIONPATTERN);
	wcex.lpszClassName = szWindowClass;
	wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

	return RegisterClassExW(&wcex);
}
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow) {
	g_hInstance = hInstance; // 인스턴스 핸들을 전역 변수에 저장합니다.

	RECT clientRect = { 0, 0, g_whClientSize.w - 1, g_whClientSize.h - 1 };
	AdjustWindowRect(&clientRect, WS_OVERLAPPEDWINDOW, true);

	HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, 0, clientRect.right - clientRect.left, clientRect.bottom - clientRect.top, nullptr, nullptr, hInstance, nullptr);

	HWND hDlg = CreateDialog(hInstance, MAKEINTRESOURCE(IDD_DIALOG1), hWnd, DlgProc);

	g_hWnd = hWnd;
	g_hDlg = hDlg;

	if (!hWnd) {
		return FALSE;
	}

	return TRUE;
}
INT_PTR CALLBACK DlgProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
	switch (message) {
	case WM_INITDIALOG:
	{
		// ===== List에 컬럼 추가 =====
		// Sprite list
		g_hActionList = GetDlgItem(hWnd, IDC_LIST2);

		// List 속성의 View를 Report로 설정해야 컬럼을 추가할 수 있다.
		const char *listColumnNames[] = { "type", "x", "y", "time" };
		UINT columnCount = sizeof(listColumnNames) / sizeof(char *);

		LVCOLUMN col = {};
		col.mask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM;
		col.fmt = LVCFMT_LEFT;
		col.cx = 50;
		char columnName[99];
		for (size_t i = 0; i < columnCount; i++) {
			strcpy_s(columnName, listColumnNames[i]);
			col.pszText = columnName;
			ListView_InsertColumn(g_hActionList, i, &col); // 컬럼 추가1
		}
		// ===== List에 컬럼 추가 ===== end

		// ===== List 뷰 설정 =====
		// 기본값은 첫 번째 서브아이템의 텍스트 영역만 선택됨
		ListView_SetExtendedListViewStyle(
			g_hActionList,
			LVS_EX_FULLROWSELECT // 아이템 전체가 클릭되도록 한다.
			| LVS_EX_GRIDLINES // 서브아이템 사이에 그리드 라인을 넣는다.
		);
		// ===== List 뷰 설정 ===== end
	}
	break;
	case WM_COMMAND:
	{
		int wmId = LOWORD(wParam);
		// 메뉴 선택을 구문 분석합니다:
		switch (wmId) {

		case IDC_BUTTON1: // PlayStop
		{
			g_cActionPattern.PlayStop(!g_cActionPattern.bPlaying);
			if (g_cActionPattern.bPlaying) {
				SetDlgItemText(g_hDlg, IDC_BUTTON1, "Stop");
			}
			else {
				SetDlgItemText(g_hDlg, IDC_BUTTON1, "Play");
				g_cActionPattern.InitUnit();
			}
		}
		break;

		case IDC_BUTTON2: // Load
		{
			//OPENFILENAME ofn = { 0, };
			char filePath[MAX_PATH] = {};
			if (OpenFileDialog(filePath)) {
				g_cActionPattern.cActionList.LoadActionPatternFile(filePath);
				UpdateActions();
			};
		}
		break;

		case IDC_BUTTON3: // Save
		{
			//OPENFILENAME ofn = { 0, };
			char filePath[MAX_PATH] = {};
			if (OpenFileDialog(filePath)) {
				g_cActionPattern.cActionList.SaveActionPatternFile(filePath);
				UpdateActions();
			};
		}
		break;

		case IDC_BUTTON4: // Delete
		{
			int selectedListItemIndex = ListView_GetNextItem(
				g_hActionList, // 윈도우 핸들
				-1, // 검색을 시작할 인덱스
				LVNI_SELECTED // 검색 조건
			);

			if (selectedListItemIndex != -1) {
				g_cActionPattern.cActionList.DeleteAction(selectedListItemIndex);
				UpdateActions();
			}
		}
		break;

		case IDC_BUTTON5: // Delete All
		{
			if (g_cActionPattern.bPlaying) break;

			g_cActionPattern.cActionList.DeleteAllActions();
			UpdateActions();
		}
		break;

		case IDC_BUTTON6: // Up
		{
			int selectedListItemIndex = ListView_GetNextItem(
				g_hActionList, // 윈도우 핸들
				-1, // 검색을 시작할 인덱스
				LVNI_SELECTED // 검색 조건
			);

			if (g_cActionPattern.cActionList.UpAction(selectedListItemIndex)) {
				UpdateActions();
			};
		}
		break;

		case IDC_BUTTON7: // Down
		{
			int selectedListItemIndex = ListView_GetNextItem(
				g_hActionList, // 윈도우 핸들
				-1, // 검색을 시작할 인덱스
				LVNI_SELECTED // 검색 조건
			);

			if (g_cActionPattern.cActionList.DownAction(selectedListItemIndex)) {
				UpdateActions();
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

	case WM_NOTIFY:
	{
		// ListView의 통지 메시지 받기
		if (wParam == IDC_LIST2) {
			NMHDR *pnmhdr = nullptr;

			// NMHDR을 얻는 방법1.
			NMTTDISPINFO *nmttdispinfo = (NMTTDISPINFO*)lParam;
			pnmhdr = &(nmttdispinfo->hdr);

			// NMHDR을 얻는 방법2.
			// NMTTDISPINFO는 NMHDR을 확장하는 구조체로서
			// 첫 멤버변수로 NMHDR을 가지고 있기 때문에 NMHDR 형으로도 캐스팅이 가능하다.
			pnmhdr = (NMHDR*)lParam;

			pnmhdr->hwndFrom; // 윈도우 핸들
			pnmhdr->idFrom; // 컨트롤 아이디
			pnmhdr->code; // 통지 코드

			if (pnmhdr->code == NM_CLICK) {
				// 클릭된 아이템 인덱스 알아내기
				g_cActionPattern.iSelectedActionIndex = ListView_GetNextItem(
					pnmhdr->hwndFrom, // 윈도우 핸들
					-1, // 검색을 시작할 인덱스
					LVNI_SELECTED // 검색 조건
				);

				// update pivot
				if (g_cActionPattern.iSelectedActionIndex != NoSelectedIndex) {

				}
			}
			else if (pnmhdr->code == LVN_ITEMCHANGED) {
				//InvalidateRect(g_hMainWnd, nullptr, true);
				g_cActionPattern.iSelectedActionIndex = ListView_GetNextItem(
					pnmhdr->hwndFrom, // 윈도우 핸들
					-1, // 검색을 시작할 인덱스
					LVNI_SELECTED // 검색 조건
				);
			}
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
	// 다이얼로그에서 닫기를 처리하면 메인 윈도우에서 처리가 안된다.
	//case WM_DESTROY:
	//{
		//PostQuitMessage(0);
	//}
	//break;
	//default:
		//return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
	switch (message) {
	case WM_COMMAND:
	{
		int wmId = LOWORD(wParam);
		// 메뉴 선택을 구문 분석합니다:
		switch (wmId) {
		case IDM_ABOUT:
			DialogBox(g_hInstance, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, DlgProc);
			break;
		case IDM_EXIT:
			DestroyWindow(hWnd);
			break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
	}
	break;

	case WM_LBUTTONDOWN:
	{
		int x = GET_X_LPARAM(lParam);
		int y = GET_Y_LPARAM(lParam);

		CAction cAction;
		cAction.sXY = { (float)x, (float)y };
		cAction.iTime = 0;
		cAction.eActionType = EActionType::EActionType_MoveTo;

		g_cActionPattern.cActionList.AddAction(cAction);

		UpdateActions();
	}
	break;

	case WM_RBUTTONDOWN:
	{
		int x = GET_X_LPARAM(lParam);
		int y = GET_Y_LPARAM(lParam);

		CAction cAction;
		cAction.sXY = { (float)x, (float)y };
		cAction.iTime = 1000;
		cAction.eActionType = EActionType::EActionType_Idle;

		g_cActionPattern.cActionList.AddAction(cAction);

		UpdateActions();
	}
	break;

	//case WM_PAINT:
	//{
	//	PAINTSTRUCT ps;
	//	HDC hdc = BeginPaint(hWnd, &ps);
	//	// TODO: 여기에 hdc를 사용하는 그리기 코드를 추가합니다...
	//	EndPaint(hWnd, &ps);
	//}
	//break;

	case WM_MOVE:
	{
		UpdateSubWndPosition();
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
// 정보 대화 상자의 메시지 처리기입니다.
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam) {
	UNREFERENCED_PARAMETER(lParam);
	switch (message) {
	case WM_INITDIALOG:
		return (INT_PTR)TRUE;

	case WM_COMMAND:
		if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL) {
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
	GetWindowRect(g_hDlg, &rectMainWnd);
	GetWindowRect(g_hDlg, &rectRightWnd);

	UINT clientW = rectRightWnd.right - rectMainWnd.left;
	UINT clientH = rectMainWnd.bottom - rectMainWnd.top;

	MoveWindow(hWnd, (screenX / 2) - (clientW / 2), (screenY / 2) - (clientH / 2), rectMainWnd.right - rectMainWnd.left, clientH, false);
	UpdateSubWndPosition();
}
void UpdateSubWndPosition() {
	RECT rectWnd;
	GetWindowRect(g_hWnd, &rectWnd);

	RECT rectDlg;
	GetWindowRect(g_hDlg, &rectDlg);
	MoveWindow(g_hDlg, rectWnd.right + 2, rectWnd.top, rectDlg.right - rectDlg.left, rectDlg.bottom - rectDlg.top, true);
}
void UpdateActions() {
	ListView_DeleteAllItems(g_hActionList);

	UINT spriteCount = g_cActionPattern.cActionList.cActions.size();
	for (size_t i = 0; i < spriteCount; i++) {
		CAction *pcAction = &g_cActionPattern.cActionList.cActions[i];
		LVITEM item = {};
		item.mask = LVIF_TEXT;
		item.iItem = i;
		item.iSubItem = 0; // 아이템을 처음 추가하므로 0번째 서브아이템을 선택한다.
		item.state;
		item.stateMask;

		char itemText[szMax_Action] = {};
		strcpy_s(itemText, szMax_Action, Const::szActionTypesAsString[pcAction->eActionType]);
		item.pszText = itemText;
		ListView_InsertItem(g_hActionList, &item); // 아이템 추가
		_itoa_s(pcAction->sXY.x, itemText, 10);
		ListView_SetItemText(g_hActionList, i, 1, itemText);
		_itoa_s(pcAction->sXY.y, itemText, 10);
		ListView_SetItemText(g_hActionList, i, 2, itemText);
		_itoa_s(pcAction->iTime, itemText, 10);
		ListView_SetItemText(g_hActionList, i, 3, itemText);
	}
}