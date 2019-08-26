// 05-Tool-Unit.cpp : 응용 프로그램에 대한 진입점을 정의합니다.
//

#include "stdafx.h"
#include "05-Tool-Unit.h"
#include "CGameFrame_Unit.h"
#include "lib.h"
#include "Commctrl.h"

#define MAX_LOADSTRING 100

// 전역 변수:
HINSTANCE hInst;                                // 현재 인스턴스입니다.
WCHAR szTitle[MAX_LOADSTRING];                  // 제목 표시줄 텍스트입니다.
WCHAR szWindowClass[MAX_LOADSTRING];            // 기본 창 클래스 이름입니다.

// 이 코드 모듈에 포함된 함수의 선언을 전달합니다:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);

//
HWND g_hWnd;
HWND g_hUnitStateAniList;
//HWND g_hAniFileList;
CGameFrame_Unit g_gfUnit;

//const char *g_szUnitStateTypeAsString[] = { "Idle"  };
const char *g_szUnitStateTypeAsString[] = { "Idle" , "Walk" };

void LoadUnit();
void SaveUnit();

void UpdateUI();
void UpdateBitmap();
void UpdateUnitStateAniList();
void UpdateAniFilePath();

INT_PTR CALLBACK DlgProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

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
	LoadStringW(hInstance, IDC_MY05TOOLUNIT, szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance);

	// 응용 프로그램 초기화를 수행합니다:
	if (!InitInstance(hInstance, nCmdShow))
	{
		return FALSE;
	}

	//HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_MY05TOOLUNIT));

	HWND hWndPicture = GetDlgItem(g_hWnd, IDC_PIC1);
	g_gfUnit.Init(g_hWnd, hWndPicture, 1000 / 90, { 800, 600 }, WindowMode::None);
	UpdateUI();

	MSG msg;

	// 기본 메시지 루프입니다:
	//while (GetMessage(&msg, nullptr, 0, 0))
	while (true)
	{
		if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)) {
			if (msg.message == WM_QUIT) {
				break;
			}
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		g_gfUnit.Update();

		//UpdateUI();
	}

	g_gfUnit.Release();

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
	wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_MY05TOOLUNIT));
	wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wcex.lpszMenuName = MAKEINTRESOURCEW(IDC_MY05TOOLUNIT);
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

	//HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
	   //CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, hInstance, nullptr);

	HWND hWnd = CreateDialog(hInstance, MAKEINTRESOURCE(IDD_DIALOG1), nullptr, DlgProc);
	g_hWnd = hWnd;

	if (!hWnd)
	{
		return FALSE;
	}

	//ShowWindow(hWnd, nCmdShow);
	//UpdateWindow(hWnd);

	return TRUE;
}

INT_PTR CALLBACK DlgProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);
	switch (message)
	{
	case WM_INITDIALOG:
	{
		/*HWND hList_UnitStateType = GetDlgItem(hDlg, IDC_LIST1);
		for (size_t i = 0; i < sizeof(g_szUnitStateTypeAsString) / sizeof(char *); i++)
		{
			SendMessage(hList_UnitStateType, LB_ADDSTRING, 0, (LPARAM)g_szUnitStateTypeAsString[i]);
		}*/

		g_hUnitStateAniList = GetDlgItem(hDlg, IDC_LIST1);
		//g_hAniFileList = GetDlgItem(hDlg, IDC_LIST2);

		// ===== List에 컬럼 추가 =====
		// List 속성의 View를 Report로 설정해야 컬럼을 추가할 수 있다.
		char colText0[] = "UnitStateType";
		char colText1[] = "Ani File";

		LVCOLUMN col = {};
		col.mask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM;
		col.fmt = LVCFMT_LEFT;
		col.cx = 100;
		col.pszText = colText0;
		ListView_InsertColumn(g_hUnitStateAniList, 0, &col); // 컬럼 추가0

		col.pszText = colText1;
		ListView_InsertColumn(g_hUnitStateAniList, 1, &col); // 컬럼 추가1
		// ===== List에 컬럼 추가 ===== end

		// 기본값은 첫 번째 서브아이템의 텍스트 영역만 선택됨
		ListView_SetExtendedListViewStyle(
			g_hUnitStateAniList,
			LVS_EX_FULLROWSELECT // 아이템 전체가 클릭되도록 한다.
			| LVS_EX_GRIDLINES // 서브아이템 사이에 그리드 라인을 넣는다.
		);

		UpdateUnitStateAniList();

		return (INT_PTR)TRUE;
	}



	case WM_COMMAND:
	{
		int id = LOWORD(wParam);
		switch (id)
		{

		case IDC_EDIT1:
		{
			if (HIWORD(wParam) == EN_CHANGE) {
				char unitName[szMax_UnitName];
				GetDlgItemText(hDlg, IDC_EDIT1, unitName, szMax_UnitName);
				g_gfUnit.Unit.SetName(unitName);
			}
		}
		break;

		case IDC_BUTTON1: // Load
		{
			//char filePath[MAX_PATH] = {};
			//if (OpenFileDialog2(filePath)) {
				//SetDlgItemText(g_hWnd, IDC_EDIT1, "hihi");
			//};
		}
		break;

		case IDC_BUTTON2: // Save
		{
			char filePath[MAX_PATH] = {};
			if (OpenFileDialog(filePath)) {
				g_gfUnit.SaveUnit(filePath);
			}
		}
		break;

		case IDC_BUTTON3: // Load .ani
		{
			UINT itemIndex = ListView_GetNextItem(
				g_hUnitStateAniList, // 윈도우 핸들
				-1, // 검색을 시작할 인덱스
				LVNI_SELECTED // 검색 조건
			);
			if (itemIndex != NoSelectedIndex) {
				char filePath[MAX_PATH] = {};
				char fileTitle[MAX_PATH] = {};
				if (OpenFileDialog(filePath, fileTitle)) {
					g_gfUnit.Unit.LoadAniFile((UnitStateType)itemIndex, filePath);

					UpdateUnitStateAniList();
				};
			}
			
		}
		break;

		case IDC_BUTTON4: // Delete .ani
		{
			UINT itemIndex = ListView_GetNextItem(
				g_hUnitStateAniList, // 윈도우 핸들
				-1, // 검색을 시작할 인덱스
				LVNI_SELECTED // 검색 조건
			);

			if (itemIndex != NoSelectedIndex) {
				g_gfUnit.Unit.AniInfos[itemIndex].FilePath[0] = 0;
				g_gfUnit.Unit.AniInfos[itemIndex].FileTitle[0] = 0;

				UpdateUnitStateAniList();
			}
		}
		break;

		case IDC_BUTTON5: // Play Ani
		{
			UINT itemIndex = ListView_GetNextItem(
				g_hUnitStateAniList, // 윈도우 핸들
				-1, // 검색을 시작할 인덱스
				LVNI_SELECTED // 검색 조건
			);

			if (itemIndex != NoSelectedIndex) {
				//g_gfUnit.Unit.AniInfos[itemIndex].FilePath;

				//g_gfUnit.Unit.AniInfos[itemIndex].FileTitle[0] = 0;

				g_gfUnit.Unit.Play((UnitStateType)itemIndex);

				//UpdateUnitStateAniList();
			}
		}
		break;

		case IDC_BUTTON10: // Stop Ani
		{
			g_gfUnit.Unit.Stop();
		}
		break;

		case IDC_BUTTON6:
		{

		}
		break;
		

		case IDC_BUTTON9: // Load Bitmap
		{
			char filePath[MAX_PATH] = {};
			if (OpenFileDialog(filePath)) {
				g_gfUnit.Unit.LoadUnitBitmap(filePath);

				UpdateBitmap();
			}
		}
		break;

		default:
			break;
		}
	}
	break;

	case WM_CLOSE:
	{
		PostQuitMessage(0);
		return (INT_PTR)TRUE;
	}

	// 다이얼로그에서는 WM_DESTROY, default 처리를 하지 않는다.
	// 파일 다이얼로그창에서 이벤트가 발생하지 않아서 인터페이스 조작이 안된다.
	//case WM_DESTROY:
		//PostQuitMessage(0);
		//break;
	//default:
		//return DefWindowProc(hDlg, message, wParam, lParam);
	}
	return (INT_PTR)FALSE;
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
	case WM_PAINT:
	{
		PAINTSTRUCT ps;
		HDC hdc = BeginPaint(hWnd, &ps);
		// TODO: 여기에 hdc를 사용하는 그리기 코드를 추가합니다...
		EndPaint(hWnd, &ps);
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

void SaveUnit() {

}

void UpdateUI() {
	SetDlgItemText(g_hWnd, IDC_EDIT1, g_gfUnit.Unit.Name);
}

void UpdateBitmap() {
	HWND hWndPic2 = GetDlgItem(g_hWnd, IDC_PIC2);
	HDC hdc = GetDC(hWndPic2);
	TransparentBlt(hdc, 0, 0, 100, 100, g_gfUnit.Unit.hBitmapDC, 0, 0, 100, 100, RGB(255, 0, 0));
}

void UpdateUnitStateAniList() {
	ListView_DeleteAllItems(g_hUnitStateAniList);

	LVITEM item = {};
	item.mask = LVIF_TEXT;
	item.state;
	item.stateMask;

	for (size_t i = 0; i < UnitStateType::Count; i++)
	{
		item.iItem = i;
		item.iSubItem = 0; // 아이템을 처음 추가하므로 0번째 서브아이템을 선택한다.

		char itemText[MAX_PATH];
		strcpy_s(itemText, MAX_PATH, g_szUnitStateTypeAsString[i]);
		item.pszText = itemText;
		ListView_InsertItem(g_hUnitStateAniList, &item); // 아이템 추가0

		//item.pszText = itemText;
		//item.iSubItem = 1;
		//strcpy_s(itemText, MAX_PATH, g_gfUnit.Unit.AniInfos[i].FileTitle);
		ListView_SetItemText(g_hUnitStateAniList, i, 1, g_gfUnit.Unit.AniInfos[i].FileTitle); // 아이템 추가0
		//ListView_SetItemText(g_hUnitStateAniList, i, 1, &item); // 아이템 추가0
	}
}

//void UpdateAniFilePath() {
//	ListView_DeleteAllItems(g_hAniFileList);
//
//	LVITEM item = {};
//	item.mask = LVIF_TEXT;
//	item.iItem = 0;
//	item.iSubItem = 0; // 아이템을 처음 추가하므로 0번째 서브아이템을 선택한다.
//	item.state;
//	item.stateMask;
//
//	for (size_t i = 0; i < UnitStateType::Count; i++)
//	{
//		item.pszText = g_gfUnit.Unit.AniInfos[i].FileTitle;
//		ListView_InsertItem(g_hAniFileList, &item); // 아이템 추가0
//	}
//}