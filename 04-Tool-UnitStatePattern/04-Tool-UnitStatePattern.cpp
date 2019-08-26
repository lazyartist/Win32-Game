// 04-Tool-UnitStatePattern.cpp : 응용 프로그램에 대한 진입점을 정의합니다.
//
#include <list> 
#include <vector> 
#include "stdafx.h"
#include <windowsx.h> // GET_X_LPARAM
#include "Commctrl.h"
#include <commdlg.h>
#include "04-Tool-UnitStatePattern.h"
#include "CGameFrame_UnitStatePattern.h"

#define MAX_LOADSTRING 100

// 전역 변수:
HINSTANCE hInst;                                // 현재 인스턴스입니다.
WCHAR szTitle[MAX_LOADSTRING];                  // 제목 표시줄 텍스트입니다.
WCHAR szWindowClass[MAX_LOADSTRING];            // 기본 창 클래스 이름입니다.

WH g_whClientSize = { 800, 600 };
char g_szAniFilePath[MAX_PATH];
const char *g_szUnitStateTypeAsString[] = { "Idle" , "Walk" };

HWND g_hWnd;
HWND g_hDlg;
CGameFrame_UnitStatePattern g_gfUnitStatePattern;
HWND g_hUnitStateList;

void SetWindowPositionToCenter(HWND hWnd);
//bool OpenFileDialog(OPENFILENAME &ofn);
void UpdateSubWndPosition();
void UpdateUnitStatesList();

// 이 코드 모듈에 포함된 함수의 선언을 전달합니다:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
INT_PTR CALLBACK    DlgProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);

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
	LoadStringW(hInstance, IDC_MY04TOOLUNITSTATEPATTERN, szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance);

	// 응용 프로그램 초기화를 수행합니다:
	if (!InitInstance(hInstance, nCmdShow))
	{
		return FALSE;
	}

	g_gfUnitStatePattern.Init(g_hWnd, g_hWnd, 1000 / 90, { g_whClientSize.w, g_whClientSize.h }, WindowMode::Window);

	SetWindowPositionToCenter(g_hWnd);

	//HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_MY04TOOLUNITSTATEPATTERN));

	MSG msg;

	// 기본 메시지 루프입니다:
	while (true)
	{
		if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
		{
			if (msg.message == WM_QUIT)
			{
				break;
			}
			//if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
			{
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}

		}

		g_gfUnitStatePattern.Update();
	}

	g_gfUnitStatePattern.Release();

	return (int)msg.wParam;
}



//
//  함수: MyRegisterClass()
//
//  용도: 창 클래스를 등록합니다.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEXW wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = WndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance;
	wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_MY04TOOLUNITSTATEPATTERN));
	wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wcex.lpszMenuName = MAKEINTRESOURCEW(IDC_MY04TOOLUNITSTATEPATTERN);
	wcex.lpszClassName = szWindowClass;
	wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

	return RegisterClassExW(&wcex);
}

//
//   함수: InitInstance(HINSTANCE, int)
//
//   용도: 인스턴스 핸들을 저장하고 주 창을 만듭니다.
//
//   주석:
//
//        이 함수를 통해 인스턴스 핸들을 전역 변수에 저장하고
//        주 프로그램 창을 만든 다음 표시합니다.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
	hInst = hInstance; // 인스턴스 핸들을 전역 변수에 저장합니다.

	RECT clientRect = { 0, 0, g_whClientSize.w - 1, g_whClientSize.h - 1 };
	AdjustWindowRect(&clientRect, WS_OVERLAPPEDWINDOW, true);

	HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, 0, clientRect.right - clientRect.left, clientRect.bottom - clientRect.top, nullptr, nullptr, hInstance, nullptr);

	HWND hDlg = CreateDialog(hInstance, MAKEINTRESOURCE(IDD_DIALOG1), hWnd, DlgProc);

	g_hWnd = hWnd;
	g_hDlg = hDlg;

	if (!hWnd)
	{
		return FALSE;
	}

	//ShowWindow(hWnd, nCmdShow);
	//UpdateWindow(hWnd);

	return TRUE;
}
INT_PTR CALLBACK DlgProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_INITDIALOG:
	{
		// list
		// ===== List에 컬럼 추가 =====
		// Sprite list
		g_hUnitStateList = GetDlgItem(hWnd, IDC_LIST2);

		// List 속성의 View를 Report로 설정해야 컬럼을 추가할 수 있다.
		const char *listColumnNames[] = { "type", "x", "y", "time" };
		UINT columnCount = sizeof(listColumnNames) / sizeof(char *);

		LVCOLUMN col = {};
		col.mask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM;
		col.fmt = LVCFMT_LEFT;
		col.cx = 50;
		char columnName[99];
		for (size_t i = 0; i < columnCount; i++)
		{
			strcpy_s(columnName, listColumnNames[i]);
			col.pszText = columnName;
			ListView_InsertColumn(g_hUnitStateList, i, &col); // 컬럼 추가1
		}
		// ===== List에 컬럼 추가 ===== end


		// ===== List 뷰 설정 =====
		// 기본값은 첫 번째 서브아이템의 텍스트 영역만 선택됨
		ListView_SetExtendedListViewStyle(
			g_hUnitStateList,
			LVS_EX_FULLROWSELECT // 아이템 전체가 클릭되도록 한다.
			| LVS_EX_GRIDLINES // 서브아이템 사이에 그리드 라인을 넣는다.
		);
		// ===== List 뷰 설정 ===== end

		//SetTimer(hWnd, 1, 1000/30, nullptr);
	}
	break;

	//case WM_TIMER:
	//{
	//	//g_cUnitStatePattern.Update();
	//}
	break;

	case WM_COMMAND:
	{
		int wmId = LOWORD(wParam);
		// 메뉴 선택을 구문 분석합니다:
		switch (wmId)
		{
		
		case IDC_BUTTON1: // PlayStop
		{
			g_gfUnitStatePattern.PlayStop(!g_gfUnitStatePattern.IsPlaying);

			if (g_gfUnitStatePattern.IsPlaying) {
				SetDlgItemText(g_hDlg, IDC_BUTTON1, "Stop");
			}
			else {
				SetDlgItemText(g_hDlg, IDC_BUTTON1, "Play");
				g_gfUnitStatePattern.InitUnit();
			}
		}
		break;

		case IDC_BUTTON2: // Load
		{
			//OPENFILENAME ofn = { 0, };
			char filePath[MAX_PATH] = {};
			if (OpenFileDialog(filePath)) {
				g_gfUnitStatePattern.UnitStatePattern.LoadUnitStatePatternFile(filePath);
				UpdateUnitStatesList();
			};
		}
		break;

		case IDC_BUTTON3: // Save
		{
			//OPENFILENAME ofn = { 0, };
			char filePath[MAX_PATH] = {};
			if (OpenFileDialog(filePath)) {
				g_gfUnitStatePattern.UnitStatePattern.SaveUnitStatePatternFile(filePath);
				UpdateUnitStatesList();
			};
		}
		break;

		case IDC_BUTTON4: // Delete
		{
			int selectedListItemIndex = ListView_GetNextItem(
				g_hUnitStateList, // 윈도우 핸들
				-1, // 검색을 시작할 인덱스
				LVNI_SELECTED // 검색 조건
			);

			if (selectedListItemIndex != -1) {
				g_gfUnitStatePattern.UnitStatePattern.DeleteUnitState(selectedListItemIndex);
				UpdateUnitStatesList();
			}
		}
		break;

		case IDC_BUTTON5: // Delete All
		{
			if (g_gfUnitStatePattern.IsPlaying) break;

			g_gfUnitStatePattern.UnitStatePattern.DeleteAllUnitState();
			UpdateUnitStatesList();
		}
		break;

		case IDC_BUTTON6: // Up
		{
			int selectedListItemIndex = ListView_GetNextItem(
				g_hUnitStateList, // 윈도우 핸들
				-1, // 검색을 시작할 인덱스
				LVNI_SELECTED // 검색 조건
			);

			if (g_gfUnitStatePattern.UnitStatePattern.UpUnitState(selectedListItemIndex)) {
				UpdateUnitStatesList();
			};
		}
		break;

		case IDC_BUTTON7: // Down
		{
			int selectedListItemIndex = ListView_GetNextItem(
				g_hUnitStateList, // 윈도우 핸들
				-1, // 검색을 시작할 인덱스
				LVNI_SELECTED // 검색 조건
			);

			if (g_gfUnitStatePattern.UnitStatePattern.DownUnitState(selectedListItemIndex)) {
				UpdateUnitStatesList();
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
				g_gfUnitStatePattern.SelectedUnitStateIndex = ListView_GetNextItem(
					pnmhdr->hwndFrom, // 윈도우 핸들
					-1, // 검색을 시작할 인덱스
					LVNI_SELECTED // 검색 조건
				);

				// update pivot
				if (g_gfUnitStatePattern.SelectedUnitStateIndex != NoSelectedIndex) {
					
				}
			}
			else if (pnmhdr->code == LVN_ITEMCHANGED) {
				//InvalidateRect(g_hMainWnd, nullptr, true);
				g_gfUnitStatePattern.SelectedUnitStateIndex = ListView_GetNextItem(
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

//
//  함수: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  용도: 주 창의 메시지를 처리합니다.
//
//  WM_COMMAND  - 응용 프로그램 메뉴를 처리합니다.
//  WM_PAINT    - 주 창을 그립니다.
//  WM_DESTROY  - 종료 메시지를 게시하고 반환합니다.
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_COMMAND:
	{
		int wmId = LOWORD(wParam);
		// 메뉴 선택을 구문 분석합니다:
		switch (wmId)
		{
		case IDM_ABOUT:
			DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
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

		UnitState unitState;
		unitState.XY = { (float)x, (float)y };
		unitState.Time = 0;
		unitState.UnitStateType = UnitStateType::Walk;

		g_gfUnitStatePattern.UnitStatePattern.AddUnitState(unitState);

		UpdateUnitStatesList();
	}
	break;

	case WM_RBUTTONDOWN:
	{
		int x = GET_X_LPARAM(lParam);
		int y = GET_Y_LPARAM(lParam);

		UnitState unitState;
		unitState.XY = { (float)x, (float)y };
		unitState.Time = 1000;
		unitState.UnitStateType = UnitStateType::Idle;

		g_gfUnitStatePattern.UnitStatePattern.AddUnitState(unitState);

		UpdateUnitStatesList();
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

// 윈도우의 위치를 스크린 가운데로 옮김
void SetWindowPositionToCenter(HWND hWnd) {
	UpdateSubWndPosition();

	int screenX = GetSystemMetrics(SM_CXFULLSCREEN);
	int screenY = GetSystemMetrics(SM_CYFULLSCREEN);

	RECT rectMainWnd;
	RECT rectRightWnd;
	//RECT rectBottomWnd;
	GetWindowRect(g_hWnd, &rectMainWnd);
	GetWindowRect(g_hDlg, &rectRightWnd);
	//GetWindowRect(g_hBottomWnd, &rectBottomWnd);

	UINT clientW = rectRightWnd.right - rectMainWnd.left;
	UINT clientH = rectMainWnd.bottom - rectMainWnd.top;

	MoveWindow(hWnd, (screenX / 2) - (clientW / 2), (screenY / 2) - (clientH / 2), rectMainWnd.right - rectMainWnd.left, clientH, false);
	//MoveWindow(hWnd, (screenX / 2) - (clientW / 2), (screenY / 2) - (clientH / 2), clientW, clientH, false);

	UpdateSubWndPosition();
}

void UpdateSubWndPosition() {
	RECT rectWnd;
	GetWindowRect(g_hWnd, &rectWnd);

	RECT rectDlg;
	GetWindowRect(g_hDlg, &rectDlg);
	MoveWindow(g_hDlg, rectWnd.right + 2, rectWnd.top, rectDlg.right - rectDlg.left, rectDlg.bottom - rectDlg.top, true);
}

void UpdateUnitStatesList() {
	ListView_DeleteAllItems(g_hUnitStateList);

	UINT spriteCount = g_gfUnitStatePattern.UnitStatePattern.UnitStates.size();
	//UINT spriteCount = g_gfUnitStatePattern.UnitStates.size();
	for (size_t i = 0; i < spriteCount; i++)
	{
		UnitState *unitState = &g_gfUnitStatePattern.UnitStatePattern.UnitStates[i];
		LVITEM item = {};
		item.mask = LVIF_TEXT;
		item.iItem = i;
		item.iSubItem = 0; // 아이템을 처음 추가하므로 0번째 서브아이템을 선택한다.
		item.state;
		item.stateMask;

		char itemText[szMax_Pos] = {};
		//_itoa_s(unitState->UnitStateType, itemText, 10);
		strcpy_s(itemText, szMax_Pos, g_szUnitStateTypeAsString[unitState->UnitStateType]);
		item.pszText = itemText;
		ListView_InsertItem(g_hUnitStateList, &item); // 아이템 추가

		//ListView_SetItemText(g_hUnitStateList, i, 0, itemText);

		_itoa_s(unitState->XY.x, itemText, 10);
		ListView_SetItemText(g_hUnitStateList, i, 1, itemText);

		_itoa_s(unitState->XY.y, itemText, 10);
		ListView_SetItemText(g_hUnitStateList, i, 2, itemText);

		_itoa_s(unitState->Time, itemText, 10);
		ListView_SetItemText(g_hUnitStateList, i, 3, itemText);
	}
}