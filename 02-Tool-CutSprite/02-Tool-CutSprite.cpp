#include "stdafx.h"
#include <windowsx.h> // GET_X_LPARAM
#include <commdlg.h> // GetOpenFileName()
#include <iostream> // sprintf_s()
#include <vector>
#include "Commctrl.h"
#include "02-Tool-CutSprite.h"
#include "common.h"
#include "CutImage.h"

#define MAX_LOADSTRING 100

int g_nScrollbarWidth;
POINT g_whMainWndSize = { 1000, 800 };
POINT g_whBottomWndSize = { 800, 250 };
POINT g_whRightWndSize = { 0, 0 }; // 리소스에 의해 결정

// 전역 변수:
HINSTANCE g_hInst;                                // 현재 인스턴스입니다.

// original bitmap
HDC g_hBitmapSrcDC;
BITMAP g_bitmapSrcHeader;

// buffer dc
HDC g_hBufferMemDC_MainWnd;
HDC g_hBufferMemDC_BottomWnd;


HWND g_hSpriteList = nullptr;
HWND g_hCollisionList = nullptr;

HDC g_hAniBufferMemDC;

bool g_bIsLBDrag = false;
RECT g_rectLBDrag = { 0, 0, 0, 0 };
POINT g_pntScrollPos = { 0, 0 };
bool g_bIsRBDrag = false;
RECT g_rectRBDrag = { 0, 0, 0, 0 };
POINT g_pntScrollPosWhenRButtonDown = { 0, 0 };

vector<SpriteInfo> g_vSpriteInfos;
SpriteInfo g_SpriteInfo;
INT g_SpriteInfoIndex = -1;

HWND g_hMainWnd;                                  // 메인 윈도우
HWND g_hRightWnd;                                 // 오른쪽 윈도우
HWND g_hBottomWnd;                                // 하단 윈도우

// ===== Resource =====
WCHAR g_szMainWndTitle[] = L"Main";               // 제목 표시줄 텍스트
WCHAR g_szBottomWndTitle[] = L"Bottom";           // 제목 표시줄 텍스트

WCHAR g_szMainWndClass[] = L"MainWnd";            // 메인창 클래스 이름
WCHAR g_szBottomWndClass[] = L"BottomWnd";        // 하단창 클래스 이름

const UINT g_nRightDlgId = IDD_DIALOG1;           // 우측 다이얼로그 리소스 ID
const UINT g_nMenuId = IDC_MY02TOOLCUTSPRITE;	  // 메뉴 ID
const UINT g_nIconId = IDI_MY02TOOLCUTSPRITE;	  // 아이콘 ID
const UINT g_nSmallIconId = IDI_SMALL;			  // 작은 아이콘 ID
const UINT g_nMenuId_OpenFile = ID_32771;		  // 파일 열기 메뉴 ID
const UINT g_nMenuId_SaveFile = ID_32772;		  // 파일 저장 메뉴 ID
// ===== Resource ===== end

const UINT g_nMainWndTimerId = 1;
const UINT g_nBottomWndTimerId = 10;

// animation
bool g_bAniPlay = false;
DWORD g_nAniPrevTime = 0; // milliseconds
UINT g_nAniSpriteIndex = 0;

char g_szImageFileName[MAX_PATH] = {};
//char g_szImageFileName[] = "sprites/castlevania_sm.bmp";
//char g_szImageFileName[] = "sprites/test_100_red.bmp";

MouseModeType g_MouseModeType = MouseModeType::Sprite;
BitmapViewInfo g_BitmapViewInfo;
CutImage g_CutImage;

ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);

LRESULT CALLBACK    MainWndProc(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK    BottomWndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    RightDlgProc(HWND, UINT, WPARAM, LPARAM);

void UpdateMainWndScroll();
void UpdateSubWndPosition();
void SetWindowPositionToCenter(HWND hWnd);
void AddRectToSpriteList(RECT rect);
void AddRectToCollisionList(RECT rect);
void AddMagnification(float v);
void SaveSpriteListToFile();
void LoadSpriteListFromFile();
void InitBitmapForMainWnd();
SpriteInfo GetSpriteInfo(int index);
void AddSpriteInfo(INT index, SpriteInfo *spriteInfo);
void SetPivots(HWND hDlg);
void UpdateSpriteList();
void UpdateCollisionList();

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPWSTR    lpCmdLine,
	_In_ int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

	MyRegisterClass(hInstance);

	// 초기화
	if (!InitInstance(hInstance, nCmdShow))
	{
		return FALSE;
	}

	// 프로그램 시작
	LoadSpriteListFromFile();
	InitBitmapForMainWnd();

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

	// 스크롤 넓이를 구해서 클라이언트 영역에서 빼주면 스크롤 영역이 딱 맞지가 않는다. 일단 대충 *4해서 맞춘다.
	g_nScrollbarWidth = GetSystemMetrics(SM_CXVSCROLL) * 6;
	//g_nScrollbarWidth = 0;

	// 메인 윈도우
	RECT clientRect = { 0, 0, g_whMainWndSize.x - 1, g_whMainWndSize.y - 1 };
	AdjustWindowRect(&clientRect, WS_OVERLAPPEDWINDOW, true);

	HWND hWnd = CreateWindowW(g_szMainWndClass, g_szMainWndTitle, (WS_OVERLAPPEDWINDOW & ~WS_MAXIMIZEBOX/*최대화 버튼 비활성화*/),
		0, 0, clientRect.right - clientRect.left, clientRect.bottom - clientRect.top, nullptr, nullptr, hInstance, nullptr);

	g_hMainWnd = hWnd;

	UpdateMainWndScroll();

	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);

	// 하단 윈도우
	clientRect = { 0, 0, g_whBottomWndSize.x - 1, g_whBottomWndSize.y - 1 };
	AdjustWindowRect(&clientRect, WS_OVERLAPPEDWINDOW, true);

	g_hBottomWnd = CreateWindowW(g_szBottomWndClass, g_szBottomWndTitle, (WS_POPUP | WS_CAPTION | WS_THICKFRAME | WS_VISIBLE),
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

	case WM_SETCURSOR:
		//dlog("WM_SETCURSOR", g_bIsRBDrag);
		if (g_bIsRBDrag) {
			SetCursor(LoadCursor(nullptr, IDC_SIZEALL));
			return (INT_PTR)TRUE; // 리턴 true하지 않으면 이 프로시저에서 메시지 처리에 실패했다고 생각하고 운영체제가 기본 메시지 처리를 실행한다.
		}
		else if (g_MouseModeType == MouseModeType::TransparentColor) {
			SetCursor(LoadCursor(nullptr, IDC_HAND));
		}
		else if (g_MouseModeType == MouseModeType::Pivot) {
			SetCursor(LoadCursor(nullptr, IDC_CROSS));
		}
		else if (g_MouseModeType == MouseModeType::Collision) {
			HCURSOR hCursor;
			hCursor = LoadCursor(g_hInst, MAKEINTRESOURCE(IDC_CURSOR1));
			SetCursor(hCursor);
		}
		else {
			return DefWindowProc(hWnd, message, wParam, lParam);
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
	case WM_MOUSEMOVE:
	{
		if (wParam == MK_LBUTTON) {
			if (g_bIsLBDrag) {
				float fMagnification = g_BitmapViewInfo.Magnification;

				g_rectLBDrag.right = round((GET_X_LPARAM(lParam) + g_pntScrollPos.x) / fMagnification);
				g_rectLBDrag.bottom = round((GET_Y_LPARAM(lParam) + g_pntScrollPos.y) / fMagnification);

				//InvalidateRect(hWnd, nullptr, true);
				dlog(g_rectLBDrag);
			}
		}
		else if (wParam == MK_RBUTTON) {
			if (g_bIsRBDrag) {
				float fMagnification = g_BitmapViewInfo.Magnification;

				g_rectRBDrag.right = GET_X_LPARAM(lParam);
				g_rectRBDrag.bottom = GET_Y_LPARAM(lParam);

				g_pntScrollPos.x = g_pntScrollPosWhenRButtonDown.x - (g_rectRBDrag.right - g_rectRBDrag.left);
				g_pntScrollPos.y = g_pntScrollPosWhenRButtonDown.y - (g_rectRBDrag.bottom - g_rectRBDrag.top);

				// 스크롤이 이미지의 좌상단보다 더 좌상단으로 이동하지 못하게 0보다 작을 경우 0으로 조정한다.
				// TranslationBlt() 호출 시 g_pntScrollPos.x, g_pntScrollPos.y의 부호를 바꿔서 전달하기 때문에 
				// 음수이면 양수로 전달되고 양수로 전달됐다는 건 dc의 양수좌표부터 이미지 복사를 시작한다는 뜻이기 때문에 
				// 이미지가 우하단으로 이동하게 되고 이미지의 좌상단을 보게된다.
				// 0, 0 이하의 좌표는 볼 필요가 없으므로 음수일경우 0으로 조정한다.
				if (g_pntScrollPos.x < 0) g_pntScrollPos.x = 0;
				if (g_pntScrollPos.y < 0) g_pntScrollPos.y = 0;

				// 이미지가 클라이언트 영역보다 클 경우 이동할 수 있는 최대 스크롤 범위
				int nMaxScrollX = (g_bitmapSrcHeader.bmWidth * g_BitmapViewInfo.Magnification - g_whMainWndSize.x) + g_nScrollbarWidth;
				int nMaxScrollY = (g_bitmapSrcHeader.bmHeight * g_BitmapViewInfo.Magnification - g_whMainWndSize.y) + g_nScrollbarWidth;

				// 최대 스크롤 범위가 음수라는 건 클라이언트 영역보다 이미지가 작다는 뜻이므로 
				// 스크롤 되지 않게 최대 스크롤 범위를 0으로 설정한다.
				nMaxScrollX = max(nMaxScrollX, 0);
				nMaxScrollY = max(nMaxScrollY, 0);

				if (g_pntScrollPos.x > nMaxScrollX) g_pntScrollPos.x = nMaxScrollX;
				if (g_pntScrollPos.y > nMaxScrollY) g_pntScrollPos.y = nMaxScrollY;

				SetScrollPos(hWnd, SB_HORZ, g_pntScrollPos.x, true);
				SetScrollPos(hWnd, SB_VERT, g_pntScrollPos.y, true);

				dlog("rbtn", g_pntScrollPos.x, g_pntScrollPos.y);

				//InvalidateRect(hWnd, nullptr, true);
				dlog(g_rectLBDrag);
			}
		}
		dlog(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));

	}
	break;
	case WM_LBUTTONDOWN:
	{
		if (g_MouseModeType == MouseModeType::TransparentColor) {
			int x = GET_X_LPARAM(lParam);
			int y = GET_Y_LPARAM(lParam);

			HDC hdc = GetDC(hWnd);
			COLORREF color = GetPixel(hdc, x, y);
			g_BitmapViewInfo.TransparentColor = color;
			DeleteDC(hdc);
		}
		else if (g_MouseModeType == MouseModeType::Pivot) {
			// 확대된 상태의 좌표이기 때문에(g_pntScrollPos도 확대된 좌표) 배율로 나눠준다.
			int x = (GET_X_LPARAM(lParam) + g_pntScrollPos.x) / g_BitmapViewInfo.Magnification;
			int y = (GET_Y_LPARAM(lParam) + g_pntScrollPos.y) / g_BitmapViewInfo.Magnification;

			// set pivot
			x -= g_SpriteInfo.Rect.left;
			y -= g_SpriteInfo.Rect.top;

			g_SpriteInfo.Pivot.x = x;
			g_SpriteInfo.Pivot.y = y;

			char szX[szMax_Pos];
			char szY[szMax_Pos];
			_itoa_s(x, szX, szMax_Pos, 10);
			_itoa_s(y, szY, szMax_Pos, 10);
			HWND hList = GetDlgItem(g_hRightWnd, IDC_LIST1);
			ListView_SetItemText(hList, g_SpriteInfoIndex, 5, szX);
			ListView_SetItemText(hList, g_SpriteInfoIndex, 6, szY);

			InvalidateRect(g_hRightWnd, nullptr, true);
		}
		else {
			g_bIsLBDrag = true;

			// 멀티 모니터에서 부정확한 좌표를 반환하기 때문에 GET_X_LPARAM, GET_Y_LPARAM를 사용
			//rect.left = LOWORD(lParam);
			//rect.top = HIWORD(lParam);

			g_rectLBDrag.left = round(GET_X_LPARAM(lParam));
			g_rectLBDrag.top = round(GET_Y_LPARAM(lParam));
			g_rectLBDrag.right = g_rectLBDrag.left;
			g_rectLBDrag.bottom = g_rectLBDrag.top;

			//log(g_rectDrag.left, g_rectDrag.top);

			// 클라이언트 영역에서 스크롤을 감안하여 드래그 위치를 계산하고 확대/축소 되지 않은 원본을 위해 배율로 나눠준다.
			g_rectLBDrag = (g_rectLBDrag + g_pntScrollPos) / g_BitmapViewInfo.Magnification;
		}
	}
	break;
	case WM_LBUTTONUP:
	{
		if (g_MouseModeType == MouseModeType::TransparentColor) {
			g_MouseModeType = MouseModeType::Sprite;
			InvalidateRect(g_hRightWnd, nullptr, false);
		}
		else if (g_MouseModeType == MouseModeType::Pivot) {
			g_MouseModeType = MouseModeType::Sprite;
			InvalidateRect(g_hRightWnd, nullptr, false);
		}
		else {
			g_bIsLBDrag = false;

			float fMagnification = g_BitmapViewInfo.Magnification;

			auto x = GET_X_LPARAM(lParam);
			auto y = GET_Y_LPARAM(lParam);
			dlog("up x,y = ", x, y);

			g_rectLBDrag.right = round((GET_X_LPARAM(lParam) + g_pntScrollPos.x) / fMagnification);
			g_rectLBDrag.bottom = round((GET_Y_LPARAM(lParam) + g_pntScrollPos.y) / fMagnification);

			// top과 left가 bottom과 right보다 항상 작도록 수정
			if (g_rectLBDrag.right < g_rectLBDrag.left) {
				LONG temp = g_rectLBDrag.left;
				g_rectLBDrag.left = g_rectLBDrag.right;
				g_rectLBDrag.right = temp;
			}
			if (g_rectLBDrag.bottom < g_rectLBDrag.top) {
				LONG temp = g_rectLBDrag.top;
				g_rectLBDrag.top = g_rectLBDrag.bottom;
				g_rectLBDrag.bottom = temp;
			}

			dlog("up rect. = ", x, y);

			g_rectLBDrag = g_CutImage.FitToImage(g_rectLBDrag, g_BitmapViewInfo.TransparentColor);

			// 드래그 영역을 자동으로 리스트에 추가
			UINT isAutoAddDragRectToList = IsDlgButtonChecked(g_hRightWnd, IDC_CHECK1); // 버튼 컨트롤러 체크 상태 가져옴
			if (isAutoAddDragRectToList && g_rectLBDrag.left != g_rectLBDrag.right) {
				AddRectToSpriteList(g_rectLBDrag);
			}

			//InvalidateRect(hWnd, nullptr, true);
		}
	}
	break;
	case WM_RBUTTONDOWN:
	{
		g_bIsRBDrag = true;
		g_pntScrollPosWhenRButtonDown = g_pntScrollPos;

		auto x = GET_X_LPARAM(lParam);
		auto y = GET_Y_LPARAM(lParam);
		dlog("r d x,y = ", x, y);
		g_rectRBDrag = { x, y, x, y };
		SetCursor(LoadCursor(nullptr, IDC_SIZEALL));
	}
	break;
	case WM_RBUTTONUP:
	{
		g_bIsRBDrag = false;
		SetCursor(LoadCursor(nullptr, IDC_SIZEALL));
	}
	break;

	case WM_TIMER:
	{
		float fMagnification = g_BitmapViewInfo.Magnification;

		HDC hdc = GetDC(hWnd);

		// 백버퍼용 DC를 매번 생성하지 않을 경우 화면은 지우고 다시 그려야하는데 이 때 사용할 수 있는 명령어가 PatBlt() 이다.
		RECT crt;
		GetClientRect(hWnd, &crt);
		FillRect(g_hBufferMemDC_MainWnd, &crt, GetSysColorBrush(COLOR_WINDOW));

		// bitmap
		// 버퍼 DC를 원본 비트맵 DC로 덮어쓰며 확대/축소한다.
		// 단순 복사(BitBlt), 확대/축소 복사(StretchBlt)
		// 확대/축소, 지정색 제거 복사
		//TransparentBlt(g_hBufferMemDC, -g_pntScrollPos.x, -g_pntScrollPos.y, g_bitmapHeader.bmWidth * fMagnification, g_bitmapHeader.bmHeight * fMagnification, g_hBitmapSrcDC, 0, 0, g_bitmapHeader.bmWidth, g_bitmapHeader.bmHeight, RGB(255, 0, 0));

		// 화면에 보이는 비트맵만 복사해서 속도를 높이려 했는데 원본 비트맵 영역을 벗어나면 흰화면이 나온다. 추후 개선
		TransparentBlt(g_hBufferMemDC_MainWnd, 0, 0,
			g_bitmapSrcHeader.bmWidth,
			g_bitmapSrcHeader.bmHeight,

			g_hBitmapSrcDC,
			g_pntScrollPos.x / fMagnification,
			g_pntScrollPos.y / fMagnification,
			(g_bitmapSrcHeader.bmWidth / fMagnification),
			(g_bitmapSrcHeader.bmHeight / fMagnification),

			RGB(255, 0, 0));

		// draw rect
		//SetROP2(hdc, R2_BLACK); // 외곽은 검은색, 내부는 비어있는 사각형
		SetROP2(g_hBufferMemDC_MainWnd, R2_MASKPEN); // 외곽은 검은색, 내부는 비어있는 사각형

		// Rectangle 고려사항
		// 1. Rectangle은 left, top좌표에 해당하는 픽셀에 라인을 그리지만 right, bottom은 해당하는 픽셀 - 1 위치에 라인을 그린다.
		// 따라서 right, bottom에 +1 한다.
		// 2. 확대 시 픽셀이 1x1 -> 2x2 -> 3x3 으로 확대되기 때문에 드래그 영역을 배수곱만 하면 확대된 픽셀의 첫번째 픽셀에 그려진다.
		// 따라서 확대된 픽셀의 바깥쪽에 라인을 그리려면 배수를 더해주는데 2배이상부터 보정해야하므로 배율에서 -1한 값을 더해준다.
		UINT pixelOffset = fMagnification - 1;
		char szItem[szMax_Pos];
		RECT spriteRect;
		SpriteInfo spriteInfo;

		int itemCount = g_vSpriteInfos.size();
		int selectedListItemIndex = ListView_GetNextItem(
			g_hSpriteList, // 윈도우 핸들
			-1, // 검색을 시작할 인덱스
			LVNI_SELECTED // 검색 조건
		);

		HPEN hPen = CreatePen(PS_SOLID, 1, RGB(0, 0, 0));
		HPEN hSelectedPen = CreatePen(PS_DASHDOT, 1, RGB(0, 255, 0));
		HPEN hOldPen = nullptr;
		for (size_t i = 0; i < itemCount; i++)
		{
			spriteInfo = GetSpriteInfo(i);
			spriteRect = spriteInfo.Rect;

			// 선택된 Sprite는 외곽선의 색상을 다르게한다.
			if (selectedListItemIndex == i) {
				hOldPen = (HPEN)SelectObject(g_hBufferMemDC_MainWnd, hSelectedPen);
				//SetROP2(hdc, R2_MASKPEN); // 외곽은 검은색, 내부는 비어있는 사각형
				SetROP2(g_hBufferMemDC_MainWnd, R2_MERGENOTPEN); // 외곽은 핑크, 내부는 비어있는 사각형
			}
			else {
				SetROP2(g_hBufferMemDC_MainWnd, R2_MASKPEN); // 외곽은 검은색, 내부는 비어있는 사각형
			}

			// sprite rect
			spriteRect = spriteRect * fMagnification;
			spriteRect = spriteRect - g_pntScrollPos;
			Rectangle(g_hBufferMemDC_MainWnd, spriteRect.left, spriteRect.top, spriteRect.right + 1 + pixelOffset, spriteRect.bottom + 1 + pixelOffset);

			// sprite pivot
			SetROP2(g_hBufferMemDC_MainWnd, R2_NOTXORPEN); // 외곽은 반전색, 내부는 비어있는 사각형
			XY xyPivot = { spriteInfo.Rect.left + spriteInfo.Pivot.x, spriteInfo.Rect.top + spriteInfo.Pivot.y };
			xyPivot = xyPivot * fMagnification;
			xyPivot = xyPivot - g_pntScrollPos;
			MoveToEx(g_hBufferMemDC_MainWnd, xyPivot.x - nPivotHalfSize, xyPivot.y, nullptr);
			LineTo(g_hBufferMemDC_MainWnd, xyPivot.x + nPivotHalfSize, xyPivot.y);
			MoveToEx(g_hBufferMemDC_MainWnd, xyPivot.x, xyPivot.y - nPivotHalfSize, nullptr);
			LineTo(g_hBufferMemDC_MainWnd, xyPivot.x, xyPivot.y + nPivotHalfSize);


			//if (selectedListItemIndex == i) {
			if (true) {
				(HPEN)SelectObject(g_hBufferMemDC_MainWnd, hPen);

				for (size_t ii = 0; ii < spriteInfo.CollisionCount; ii++)
				{
					// collision rect
					RECT collisionRect = spriteInfo.Collisions[ii];
					collisionRect = collisionRect * fMagnification;
					//collisionRect = collisionRect - g_pntScrollPos;
					//Rectangle(g_hBufferMemDC, collisionRect.left, collisionRect.top, collisionRect.right + 1 + pixelOffset, collisionRect.bottom + 1 + pixelOffset);
					Rectangle(g_hBufferMemDC_MainWnd, spriteRect.left + collisionRect.left, spriteRect.top + collisionRect.top, spriteRect.left + collisionRect.right + 1 + pixelOffset, spriteRect.top + collisionRect.bottom + 1 + pixelOffset);
				}
			}

			if (hOldPen != nullptr) {
				(HPEN)SelectObject(g_hBufferMemDC_MainWnd, hOldPen);
				hOldPen = nullptr;
			}
		}
		DeleteObject(hSelectedPen);

		// 현재 드래그 영역
		if (g_rectLBDrag.left != g_rectLBDrag.right) {
			SetROP2(g_hBufferMemDC_MainWnd, R2_NOTXORPEN); // 외곽은 반전색, 내부는 비어있는 사각형
			RECT rectDrag = g_rectLBDrag * fMagnification;
			rectDrag = rectDrag - g_pntScrollPos;
			Rectangle(g_hBufferMemDC_MainWnd, rectDrag.left, rectDrag.top, rectDrag.right + 1 + pixelOffset, rectDrag.bottom + 1 + pixelOffset);
		}

		ReleaseDC(hWnd, hdc);

		// 버퍼 DC를 그려준 뒤 WM_PAINT 메시지 발생을 위해 InvalidateRect()호출한다.
		// 이때 버퍼DC 전체를 복사하므로 화면을 지울 필요가 없기 때문에 세번째 인자는 false를 전달한다.
		InvalidateRect(hWnd, nullptr, false);
	}
	break;

	case WM_PAINT:
	{
		PAINTSTRUCT ps;
		HDC hdc = BeginPaint(hWnd, &ps);

		// 버퍼DC를 화면DC에 복사한다.
		BitBlt(hdc, 0, 0, g_bitmapSrcHeader.bmWidth, g_bitmapSrcHeader.bmHeight, g_hBufferMemDC_MainWnd, 0, 0, SRCCOPY);

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
	case WM_VSCROLL:
	{
		int vScrollPos = 0;

		switch (LOWORD(wParam))
		{
		case SB_LINELEFT:
			break;
		case SB_THUMBTRACK:
		{
			vScrollPos = HIWORD(wParam);
			dlog("WM_VSCROLL", LOWORD(wParam), vScrollPos);

			SetScrollPos(hWnd, SB_VERT, vScrollPos, true);

			int yAmount = g_pntScrollPos.y - vScrollPos;
			yAmount *= g_BitmapViewInfo.Magnification;
			g_pntScrollPos.y = vScrollPos;

			// ScrollWindow를 호출하지 않을 경우 InvalidateRect를 호출해야한다.
			//InvalidateRect(hWnd, nullptr, true);

			//ScrollWindow(hWnd, -hScrollPos, 0, nullptr, nullptr);
			return 0;
		}
		break;
		default:
			break;
		}
	}
	break;

	case WM_MOUSEWHEEL:
	{
		if (LOWORD(wParam) == MK_CONTROL) {
			int wheel = (SHORT)(HIWORD(wParam));
			(wheel > 0) ? AddMagnification(1.0f) : AddMagnification(-1.0f);
			dlog("WM_MOUSEWHEEL", wheel);
		}
	}
	break;

	case WM_HSCROLL:
	{
		int hScrollPos = 0;

		switch (LOWORD(wParam))
		{
		case SB_LINELEFT:
			break;
		case SB_THUMBTRACK:
		{
			hScrollPos = HIWORD(wParam);
			dlog("WM_HSCROLL", LOWORD(wParam), hScrollPos);

			SetScrollPos(hWnd, SB_HORZ, hScrollPos, true);

			int xAmount = g_pntScrollPos.x - hScrollPos;
			xAmount *= g_BitmapViewInfo.Magnification;
			g_pntScrollPos.x = hScrollPos;

			// ScrollWindow 호출 시 클라이언트 리페인트 메시지가 자동으로 발생한다.
			// ScrollWindow(hWnd, xAmount, 0, nullptr, nullptr);
			// UpdateWindow(hWnd); // WM_PAINT 발생
			return 0;
		}
		break;
		default:
			break;
		}
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

		SetTimer(g_hMainWnd, g_nMainWndTimerId, 1000 / nFrameRate, nullptr);
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

	case WM_TIMER:
	{
		DWORD g_nAniTime = GetTickCount();

		if (!g_bAniPlay || (g_nAniTime - g_nAniPrevTime) < g_SpriteInfo.Time) {
			break;
		}
		g_nAniPrevTime = g_nAniTime;

		UINT spriteCount = g_vSpriteInfos.size();

		if (spriteCount == 0) {
			break;
		}

		UINT nAniSpriteIndex = g_nAniSpriteIndex++ % spriteCount;
		g_SpriteInfo = g_vSpriteInfos[nAniSpriteIndex];

		//dlog("WM_SETCURSOR", g_nAniPrevTime);

		HDC hdc = GetDC(hWnd);

		float fMagnification = g_BitmapViewInfo.Magnification;

		// 백버퍼용 DC를 매번 생성하지 않을 경우 화면은 지우고 다시 그려야하는데 이 때 사용할 수 있는 명령어가 PatBlt() 이다.
		RECT crt;
		GetClientRect(hWnd, &crt);
		FillRect(g_hBufferMemDC_BottomWnd, &crt, GetSysColorBrush(COLOR_WINDOW));

		// 화면 정중앙 좌표
		XY center = { g_whBottomWndSize.x / 2, g_whBottomWndSize.y / 2 };

		// 선택된 스프라이트의 크기
		WH spriteSize = { g_SpriteInfo.Rect.right - g_SpriteInfo.Rect.left , g_SpriteInfo.Rect.bottom - g_SpriteInfo.Rect.top };
		UINT w = g_SpriteInfo.Rect.right - g_SpriteInfo.Rect.left;
		UINT h = g_SpriteInfo.Rect.bottom - g_SpriteInfo.Rect.top;
		TransparentBlt(g_hBufferMemDC_BottomWnd,
			center.x - g_SpriteInfo.Pivot.x * fMagnification,
			center.y - g_SpriteInfo.Pivot.y * fMagnification,

			spriteSize.w * fMagnification,
			spriteSize.h * fMagnification,

			g_hBitmapSrcDC,
			g_SpriteInfo.Rect.left,
			g_SpriteInfo.Rect.top,
			spriteSize.w,
			spriteSize.h,

			g_BitmapViewInfo.TransparentColor);
			//RGB(255, 0, 0));


		// 가로,세로 선 그리기
		SetROP2(g_hBufferMemDC_BottomWnd, R2_NOTXORPEN); // 외곽은 반전색, 내부는 비어있는 사각형
		MoveToEx(g_hBufferMemDC_BottomWnd, 0, center.y, nullptr);
		LineTo(g_hBufferMemDC_BottomWnd, g_whBottomWndSize.x, center.y);
		MoveToEx(g_hBufferMemDC_BottomWnd, center.x, 0, nullptr);
		LineTo(g_hBufferMemDC_BottomWnd, center.x, g_whBottomWndSize.y);


		InvalidateRect(hWnd, nullptr, true);
	}
	break;

	case WM_PAINT:
	{
		PAINTSTRUCT ps;
		HDC hdc = BeginPaint(hWnd, &ps);

		BitBlt(hdc, 0, 0, g_whBottomWndSize.x, g_whBottomWndSize.y, g_hBufferMemDC_BottomWnd, 0, 0, SRCCOPY);

		//ReleaseDC(hWnd, hdc);
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


		LVCOLUMN col = {};
		col.mask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM;
		col.fmt = LVCFMT_LEFT;
		col.cx = 50;

		// ===== List에 컬럼 추가 =====
		// Sprite list
		g_hSpriteList = GetDlgItem(hDlg, IDC_LIST1);

		// List 속성의 View를 Report로 설정해야 컬럼을 추가할 수 있다.
		const char *spriteListColumnNames[] = { "time", "left", "top", "right", "bottom", "pivot x", "pivot y" };
		UINT columnCount = sizeof(spriteListColumnNames) / sizeof(char *);


		char column[99];
		for (size_t i = 0; i < columnCount; i++)
		{
			strcpy_s(column, spriteListColumnNames[i]);
			col.pszText = column;
			ListView_InsertColumn(g_hSpriteList, i, &col); // 컬럼 추가1
		}
		// ===== List에 컬럼 추가 ===== end


		// ===== List 뷰 설정 =====
		// 기본값은 첫 번째 서브아이템의 텍스트 영역만 선택됨
		ListView_SetExtendedListViewStyle(
			g_hSpriteList,
			LVS_EX_FULLROWSELECT // 아이템 전체가 클릭되도록 한다.
			| LVS_EX_GRIDLINES // 서브아이템 사이에 그리드 라인을 넣는다.
		);
		// ===== List 뷰 설정 ===== end


		// Collision List
		g_hCollisionList = GetDlgItem(hDlg, IDC_LIST2);

		// ===== Collision List에 컬럼 추가 =====
		const char *collisionListColumnNames[] = { "left", "top", "right", "bottom" };
		columnCount = sizeof(collisionListColumnNames) / sizeof(char *);
		for (size_t i = 0; i < 4; i++)
		{
			strcpy_s(column, collisionListColumnNames[i]);
			col.pszText = column;
			ListView_InsertColumn(g_hCollisionList, i, &col); // 컬럼 추가1
		}
		// ===== Collision List에 컬럼 추가 ===== end

		// ===== Collision List 뷰 설정 =====
		// 기본값은 첫 번째 서브아이템의 텍스트 영역만 선택됨
		ListView_SetExtendedListViewStyle(
			g_hCollisionList,
			LVS_EX_FULLROWSELECT // 아이템 전체가 클릭되도록 한다.
			| LVS_EX_GRIDLINES // 서브아이템 사이에 그리드 라인을 넣는다.
		);
		// ===== Collision List 뷰 설정 ===== end

		// ===== ComboBox =====
		const char *pivots[] = { "LT", "RT", "LB", "RB", "CC" };
		HWND hCombo = GetDlgItem(hDlg, IDC_COMBO1);
		UINT pivotCount = sizeof(pivots) / sizeof(char *);
		for (size_t i = 0; i < pivotCount; i++)
		{
			ComboBox_InsertString(
				hCombo,
				i, // 삽입할 인덱스, 아이템 개수보다 높은 인덱스이면 무시됨
				pivots[i]
			);
		}
		ComboBox_SetCurSel(hCombo, 0);

		// ===== ComboBox ===== end
	}
	return (INT_PTR)TRUE;

	case WM_PAINT:
	{
		PAINTSTRUCT ps;
		HDC hdc = BeginPaint(hDlg, &ps);

		HBRUSH hBrush = CreateSolidBrush(g_BitmapViewInfo.TransparentColor);
		HBRUSH hOldBrush = (HBRUSH)SelectObject(hdc, hBrush);

		RECT rectColorPickButton;
		GetWindowRect(GetDlgItem(hDlg, IDC_BUTTON3), &rectColorPickButton); // 컨트롤의 스크린 좌표 얻기
		POINT ptColorPickButton = { rectColorPickButton.left, rectColorPickButton.top };
		ScreenToClient(hDlg, &ptColorPickButton); // 컨트롤의 스크린 좌표를 클라이언트 좌표로 변경
		Rectangle(hdc, ptColorPickButton.x + 100, ptColorPickButton.y, ptColorPickButton.x + 150, ptColorPickButton.y + 25);

		char cMagnification[9];
		_itoa_s(g_BitmapViewInfo.Magnification, cMagnification, 9, 10);
		SetDlgItemText(hDlg, IDC_EDIT1, cMagnification);

		EndPaint(hDlg, &ps);
	}

	case WM_SETCURSOR:
		if (g_MouseModeType == MouseModeType::TransparentColor) {
			SetCursor(LoadCursor(nullptr, IDC_HAND));
			return (INT_PTR)TRUE; // 리턴 true하지 않으면 이 프로시저에서 메시지 처리에 실패했다고 생각하고 운영체제가 기본 메시지 처리를 실행한다.
		}
		break;

	case WM_COMMAND:
		switch (LOWORD(wParam))
		{
		case IDC_BUTTON1: // 확대
		{
			AddMagnification(1.0f);
		}
		break;
		case IDC_BUTTON2: // 축소
		{
			AddMagnification(-1.0f);
		}
		break;
		case IDC_BUTTON3: // 컬러 선택
		{
			g_MouseModeType = MouseModeType::TransparentColor;

			SetCursor(LoadCursor(nullptr, IDC_HAND));
			//return (INT_PTR)TRUE; // 리턴 true하지 않으면 이 프로시저에서 메시지 처리에 실패했다고 생각하고 운영체제가 기본 메시지 처리를 실행한다.
		}
		break;
		case IDC_BUTTON4: // 등록
		{
			AddRectToSpriteList(g_rectLBDrag);
			//return (INT_PTR)TRUE; // 리턴 true하지 않으면 이 프로시저에서 메시지 처리에 실패했다고 생각하고 운영체제가 기본 메시지 처리를 실행한다.
		}
		break;
		case IDC_BUTTON5: // 삭제
		{
			int selectedIndex = ListView_GetNextItem(
				g_hSpriteList, // 윈도우 핸들
				-1, // 검색을 시작할 인덱스
				LVNI_SELECTED // 검색 조건
			);

			if (selectedIndex < 0) break;

			// 리스트에서 제거
			ListView_DeleteItem(g_hSpriteList, selectedIndex);

			// vector에서 제거
			auto iter = g_vSpriteInfos.begin();
			g_vSpriteInfos.erase(iter + selectedIndex);

			//InvalidateRect(g_hMainWnd, nullptr, true);
			//return (INT_PTR)TRUE; // 리턴 true하지 않으면 이 프로시저에서 메시지 처리에 실패했다고 생각하고 운영체제가 기본 메시지 처리를 실행한다.
		}
		break;
		case IDC_BUTTON6: // 파일 저장
		{
			SaveSpriteListToFile();
		}
		break;

		case IDC_BUTTON7: // 피봇 모드
		{
			g_MouseModeType = MouseModeType::Pivot;
			SetCursor(LoadCursor(nullptr, IDC_CROSS));
		}
		break;

		case IDC_BUTTON8: // 모두 삭제
		{
			HWND hList = GetDlgItem(hDlg, IDC_LIST1);
			ListView_DeleteAllItems(hList);

			g_vSpriteInfos.clear();
		}
		break;

		case IDC_BUTTON9: // 컬리전 등록
		{
			if (g_SpriteInfoIndex != NoSpriteSelect) {
				AddRectToCollisionList(g_rectLBDrag);

			}
		}
		break;

		case IDC_BUTTON10: // 컬리전 삭제
		{
			int spriteListItemIndex = ListView_GetNextItem(
				g_hSpriteList, // 윈도우 핸들
				-1, // 검색을 시작할 인덱스
				LVNI_SELECTED // 검색 조건
			);

			if (spriteListItemIndex != NoSpriteSelect) {
				int collisionListItemIndex = ListView_GetNextItem(
					g_hCollisionList, // 윈도우 핸들
					-1, // 검색을 시작할 인덱스
					LVNI_SELECTED // 검색 조건
				);

				if (collisionListItemIndex != NoSpriteSelect) {
					SpriteInfo &spriteInfo = g_vSpriteInfos[spriteListItemIndex];
					spriteInfo.RemoveCollision(collisionListItemIndex);

					g_SpriteInfo = spriteInfo;

					UpdateCollisionList();
				}

			}
		}
		break;

		case IDC_BUTTON11: // 컬리전 모두 삭제
		{
			int spriteListItemIndex = ListView_GetNextItem(
				g_hSpriteList, // 윈도우 핸들
				-1, // 검색을 시작할 인덱스
				LVNI_SELECTED // 검색 조건
			);

			if (spriteListItemIndex != NoSpriteSelect) {
				SpriteInfo &spriteInfo = g_vSpriteInfos[spriteListItemIndex];
				spriteInfo.RemoveAllCollisions();

				g_SpriteInfo = spriteInfo;

				UpdateCollisionList();
			}
		}
		break;

		case IDC_BUTTON12: // Time 설정
		{
			HWND hEditTime = GetDlgItem(hDlg, IDC_EDIT4);
			char szTime[szMax_Pos] = {};
			GetDlgItemText(g_hRightWnd, IDC_EDIT4, szTime, szMax_Pos);

			UINT time = atoi(szTime);
			if (time == 0) {
				szTime[0] = '0';
				szTime[1] = 0;
			}

			// all
			UINT spriteCount = g_vSpriteInfos.size();
			for (size_t i = 0; i < spriteCount; i++)
			{
				SpriteInfo &spriteInfo = g_vSpriteInfos[i];
				spriteInfo.Time = time;

				ListView_SetItemText(g_hSpriteList, i, 0, szTime);
			}

			int spriteListItemIndex = ListView_GetNextItem(
				g_hSpriteList, // 윈도우 핸들
				-1, // 검색을 시작할 인덱스
				LVNI_SELECTED // 검색 조건
			);
			if (spriteListItemIndex != NoSpriteSelect) {
				SpriteInfo &spriteInfo = g_vSpriteInfos[spriteListItemIndex];
				g_SpriteInfo = spriteInfo;
			}

			//if (spriteListItemIndex != NoSpriteSelect) {
			//}
		}
		break;

		case IDC_BUTTON13: // Play
		{
			g_bAniPlay = !g_bAniPlay;

			if (g_bAniPlay) {
				SetDlgItemText(hDlg, IDC_BUTTON13, "Stop");
			}
			else {
				SetDlgItemText(hDlg, IDC_BUTTON13, "Play");
			}
		}
		break;

		case IDC_BUTTON14: // Pivot 변경
		{
			SetPivots(hDlg);
			UpdateSpriteList();
		}
		break;

		default:
			break;
		}
		if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
		{
			EndDialog(hDlg, LOWORD(wParam));
			return (INT_PTR)TRUE;
		}
		break;

		// 공통 컨트롤의 통지 메시지는 WM_NOTIFY로 받는다.
	case WM_NOTIFY:
	{
		// ListView의 통지 메시지 받기
		if (wParam == IDC_LIST1) {
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
				g_SpriteInfoIndex = ListView_GetNextItem(
					pnmhdr->hwndFrom, // 윈도우 핸들
					-1, // 검색을 시작할 인덱스
					LVNI_SELECTED // 검색 조건
				);

				// update pivot
				if (g_SpriteInfoIndex != -1) {
					g_SpriteInfo = GetSpriteInfo(g_SpriteInfoIndex);

					HWND hEditX = GetDlgItem(hDlg, IDC_EDIT2);
					HWND hEditY = GetDlgItem(hDlg, IDC_EDIT3);

					char szX[szMax_Pos];
					char szY[szMax_Pos];
					_itoa_s(g_SpriteInfo.Pivot.x, szX, szMax_Pos, 10);
					_itoa_s(g_SpriteInfo.Pivot.y, szY, szMax_Pos, 10);

					SetDlgItemText(hDlg, IDC_EDIT2, szX);
					SetDlgItemText(hDlg, IDC_EDIT3, szY);
				}

				// update collision
				UpdateCollisionList();
			}
			else if (pnmhdr->code == LVN_ITEMCHANGED) {
				//InvalidateRect(g_hMainWnd, nullptr, true);
				g_SpriteInfoIndex = ListView_GetNextItem(
					pnmhdr->hwndFrom, // 윈도우 핸들
					-1, // 검색을 시작할 인덱스
					LVNI_SELECTED // 검색 조건
				);
			}
		}
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

void UpdateMainWndScroll() {
	int wBitmap = g_bitmapSrcHeader.bmWidth * g_BitmapViewInfo.Magnification;
	int hBitmap = g_bitmapSrcHeader.bmHeight * g_BitmapViewInfo.Magnification;
	bool bHorzScroll = wBitmap > (g_whMainWndSize.x - g_nScrollbarWidth);
	bool bVertScroll = hBitmap > (g_whMainWndSize.y - g_nScrollbarWidth);

	ShowScrollBar(g_hMainWnd, SB_HORZ, bHorzScroll);
	ShowScrollBar(g_hMainWnd, SB_VERT, bVertScroll);

	if (bHorzScroll) {
		SetScrollRange(g_hMainWnd, SB_HORZ, 0, wBitmap - (g_whMainWndSize.x - g_nScrollbarWidth), true);
	}
	else {
		SetScrollRange(g_hMainWnd, SB_HORZ, 0, 0, true);
	}

	if (bVertScroll) {
		SetScrollRange(g_hMainWnd, SB_VERT, 0, hBitmap - (g_whMainWndSize.y - g_nScrollbarWidth), true);
	}
	else {
		SetScrollRange(g_hMainWnd, SB_VERT, 0, 0, true);
	}
}

void UpdateSubWndPosition() {
	RECT rect;
	GetWindowRect(g_hMainWnd, &rect);
	MoveWindow(g_hRightWnd, rect.right + 2, rect.top, g_whRightWndSize.x, g_whRightWndSize.y, true);
	MoveWindow(g_hBottomWnd, rect.left, rect.bottom + 2, g_whBottomWndSize.x, g_whBottomWndSize.y, true);
}

void AddRectToSpriteList(RECT rect) {
	int itemCount = ListView_GetItemCount(g_hSpriteList);

	SpriteInfo spriteInfo;
	INT coordinates[nMax_SpriteCoordinateCount] = { rect.left, rect.top, rect.right, rect.bottom, 0, };

	spriteInfo.Time = 200;

	// rect, pivot
	spriteInfo.SetCoordinates(coordinates, nMax_SpriteCoordinateByteSize);
	spriteInfo.ResetCollisionCount();

	// collision
	int collision[nMax_RectPos] = { 0, 0, spriteInfo.Rect.right - spriteInfo.Rect.left, spriteInfo.Rect.bottom - spriteInfo.Rect.top };
	spriteInfo.AddCollision(collision, nMax_RectPos);

	AddSpriteInfo(itemCount, &spriteInfo);
}

void AddRectToCollisionList(RECT rect) {
	//int itemCount = ListView_GetItemCount(g_hCollisionList);
	SpriteInfo &spriteInfo = g_vSpriteInfos[g_SpriteInfoIndex];

	// collision
	int collision[nMax_RectPos] = { rect.left - g_SpriteInfo.Rect.left, rect.top - g_SpriteInfo.Rect.top, rect.right - rect.left, rect.bottom - rect.top };
	spriteInfo.AddCollision(collision, nMax_RectPos);

	g_SpriteInfo = spriteInfo;

	UpdateCollisionList();
}

void AddMagnification(float v) {
	g_BitmapViewInfo.Magnification += v;

	//InvalidateRect(g_hMainWnd, nullptr, true);
	InvalidateRect(g_hRightWnd, nullptr, true);

	UpdateMainWndScroll();
}

void LoadSpriteListFromFile() {
	g_vSpriteInfos.clear();

	FILE *file = nullptr;
	file = _fsopen("spritesList.cut", "rt", _SH_DENYNO);

	if (file == nullptr) return;

	HWND hList = GetDlgItem(g_hRightWnd, IDC_LIST1);

	//char szImagePath[MAX_PATH] = {};
	fgets(g_szImageFileName, MAX_PATH, file);
	RemoveCarriageReturn(g_szImageFileName);

	char szItemCount[szMax_SpriteCount] = {};
	fgets(szItemCount, szMax_SpriteCount, file);

	// item count
	int itemCount = atoi(szItemCount);

	g_vSpriteInfos.reserve(itemCount);

	// ===== List에 아이템 추가 =====
	char itemLine[szMax_PosLine] = {};
	char *token;
	char *nextToken;

	char itemText[szMax_Pos] = {};
	for (size_t i = 0; i < itemCount; i++)
	{
		memset(itemLine, 0, szMax_PosLine);
		fgets(itemLine, szMax_PosLine, file);

		// \n은 줄바꿈을 지정하는 문자이므로 순수 문자만 얻기 위해 제거한다.
		itemLine[strcspn(itemLine, "\n")] = 0; // strcspn()으로 "\n"의 위치를 찾고 그 위치에 0을 넣어준다.

		if (strnlen_s(itemLine, szMax_PosLine) == 0) continue;

		SpriteInfo spriteInfo;
		{
			nextToken = itemLine;

			token = strtok_s(nullptr, "\t", &nextToken);
			spriteInfo.Time = atoi(token);

			// rect, pivot
			int coordinates[nMax_SpriteCoordinateCount] = {  };
			for (size_t j = 0; j < nMax_SpriteCoordinateCount; j++)
			{
				token = strtok_s(nullptr, "\t", &nextToken);
				coordinates[j] = atoi(token);
			}
			spriteInfo.SetCoordinates(coordinates, sizeof(INT) * nMax_SpriteCoordinateCount);

			// collision
			token = strtok_s(nullptr, "\t", &nextToken);
			INT collisionCount = atoi(token);
			spriteInfo.ResetCollisionCount();

			for (size_t j = 0; j < collisionCount; j++)
			{
				int collisionPos[nMax_RectPos];
				for (size_t k = 0; k < nMax_RectPos; k++)
				{
					token = strtok_s(nullptr, "\t", &nextToken);
					collisionPos[k] = atoi(token);
				}

				spriteInfo.AddCollision(collisionPos, nMax_RectPos);
			}

		}

		AddSpriteInfo(i, &spriteInfo);
	}

	fclose(file);
}

void InitBitmapForMainWnd() {

	// load bitmap
	HBITMAP hBitmapSrc = (HBITMAP)LoadImage(nullptr, g_szImageFileName, IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);

	GetObject(hBitmapSrc, sizeof(BITMAP), &g_bitmapSrcHeader);

	HDC hdcMain = GetDC(g_hMainWnd);

	// 원본 Bitmap DC
	// 추후 TransparentBlt() 등으로 복사 할 때 비트맵 영역을 벗어나면 화면이 그려지지 않기 때문에 원본 비트맵 보다 더 큰 비트맵을 만들어 복사한다.
	g_hBitmapSrcDC = CreateCompatibleDC(hdcMain);
	// 메모리 DC에는 기본적으로 아주 작은 크기의 비트맵만 있기 때문에 다른 DC를 복사하겨나 그리기를 하려면 비트맵을 만들어줘야한다.
	// SelectObject()로 비트맵을 지정할 경우는 안해도 된다.
	HBITMAP hBitmapExpand = CreateCompatibleBitmap(hdcMain, g_bitmapSrcHeader.bmWidth * 2, g_bitmapSrcHeader.bmHeight * 2);
	SelectObject(g_hBitmapSrcDC, hBitmapExpand);

	// 원본 Bitmap DC에 로드한 비트맵을 복사하기 위한 임시 Bitmap DC
	HDC hBitmapSrcDCTemp = CreateCompatibleDC(hdcMain);
	SelectObject(hBitmapSrcDCTemp, hBitmapSrc); // 로드한 비트맵 적용
	DeleteObject(hBitmapSrc);

	// 원본 Bitmap DC에 비트맵 복사
	BitBlt(g_hBitmapSrcDC, 0, 0, g_bitmapSrcHeader.bmWidth, g_bitmapSrcHeader.bmHeight, hBitmapSrcDCTemp, 0, 0, SRCCOPY);

	// 메인 윈도우 BufferDC
	g_hBufferMemDC_MainWnd = CreateCompatibleDC(hdcMain);
	HBITMAP g_hBufferBitmap = CreateCompatibleBitmap(hdcMain, g_bitmapSrcHeader.bmWidth, g_bitmapSrcHeader.bmHeight);
	SelectObject(g_hBufferMemDC_MainWnd, g_hBufferBitmap);
	DeleteObject(g_hBufferBitmap);

	// 하단 윈도우 BufferDC
	HDC hdcBottom = GetDC(g_hBottomWnd);
	g_hBufferMemDC_BottomWnd = CreateCompatibleDC(hdcBottom);
	g_hBufferBitmap = CreateCompatibleBitmap(hdcBottom, g_whBottomWndSize.x, g_whBottomWndSize.y);
	SelectObject(g_hBufferMemDC_BottomWnd, g_hBufferBitmap);
	DeleteObject(g_hBufferBitmap);

	g_CutImage.Init(g_hBitmapSrcDC, g_bitmapSrcHeader);

	DeleteDC(hdcMain);

	// transparent color
	g_BitmapViewInfo.TransparentColor = GetPixel(g_hBitmapSrcDC, 0, 0);

	UpdateMainWndScroll();

	SetTimer(g_hMainWnd, g_nMainWndTimerId, 1000 / nFrameRate, nullptr);
	SetTimer(g_hBottomWnd, g_nBottomWndTimerId, 1000 / nFrameRate, nullptr);
}

void AddSpriteInfo(INT index, SpriteInfo *spriteInfo) {
	g_vSpriteInfos.push_back(*spriteInfo);

	UpdateSpriteList();

	//LVITEM item = {};
	//item.mask = LVIF_TEXT;
	//item.iItem = index;
	//item.iSubItem = 0; // 아이템을 처음 추가하므로 0번째 서브아이템을 선택한다.
	//item.state;
	//item.stateMask;

	//char itemText[szMax_Pos] = {};
	//item.pszText = itemText;
	//ListView_InsertItem(g_hSpriteList, &item); // 아이템 추가

	//_itoa_s(spriteInfo->Time, itemText, 10);
	//ListView_SetItemText(g_hSpriteList, index, 0, itemText);

	//UINT subitemCount = 1;
	//for (size_t i = 0; i < nMax_SpriteCoordinateCount; i++)
	//{
	//	_itoa_s(spriteInfo->Coordinates[i], itemText, 10);
	//	ListView_SetItemText(g_hSpriteList, index, i + subitemCount, itemText);
	//}
}

void SaveSpriteListToFile() {
	FILE *file = nullptr;
	file = _fsopen("spritesList.cut", "wt", _SH_DENYNO);

	if (file == nullptr) return;

	// image file name
	fputs(g_szImageFileName, file);
	fputs("\n", file);

	// item count
	int itemCount = g_vSpriteInfos.size();
	char szItemCount[szMax_Pos] = {};
	sprintf_s<szMax_Pos>(szItemCount, "%d\n", itemCount);
	fputs(szItemCount, file);

	auto iter = g_vSpriteInfos.begin();
	while (iter != g_vSpriteInfos.end())
	{
		// time
		fprintf_s(file, "%d", iter->Time);

		// rect, pivot
		fprintf_s(file, "\t%d\t%d\t%d\t%d\t%d\t%d", iter->Rect.left, iter->Rect.top, iter->Rect.right, iter->Rect.bottom, iter->Pivot.x, iter->Pivot.y);

		// collision
		fprintf_s(file, "\t%d", iter->CollisionCount);
		for (size_t i = 0; i < iter->CollisionCount; i++)
		{
			RECT &collision = iter->Collisions[i];
			fprintf_s(file, "\t%d\t%d\t%d\t%d", collision.left, collision.top, collision.right, collision.bottom);
		}
		fprintf_s(file, "\n");

		++iter;
	}

	fclose(file);
}

SpriteInfo GetSpriteInfo(int index) {
	return g_vSpriteInfos[index];
}

void SetPivots(HWND hDlg) {
	HWND hCombo = GetDlgItem(hDlg, IDC_COMBO1);
	char szPivotType[szMax_Pos] = {};
	ComboBox_GetText(hCombo, szPivotType, szMax_Pos);

	// 문자열이 같은지 확인
	UINT spriteCount = g_vSpriteInfos.size();
	for (size_t i = 0; i < spriteCount; i++)
	{
		SpriteInfo &spriteInfo = g_vSpriteInfos[i];
		UINT w = spriteInfo.Rect.right - spriteInfo.Rect.left;
		UINT h = spriteInfo.Rect.bottom - spriteInfo.Rect.top;

		if (strcmp(szPivotType, "LT") == 0) {
			spriteInfo.Pivot.x = 0;
			spriteInfo.Pivot.y = 0;
		}
		else if (strcmp(szPivotType, "RT") == 0) {
			spriteInfo.Pivot.x = w;
			spriteInfo.Pivot.y = 0;
		}
		else if (strcmp(szPivotType, "LB") == 0) {
			spriteInfo.Pivot.x = 0;
			spriteInfo.Pivot.y = h;
		}
		else if (strcmp(szPivotType, "RB") == 0) {
			spriteInfo.Pivot.x = w;
			spriteInfo.Pivot.y = h;
		}
		else if (strcmp(szPivotType, "CC") == 0) {
			spriteInfo.Pivot.x = w / 2;
			spriteInfo.Pivot.y = h / 2;
		}
	}
}

void UpdateSpriteList() {
	ListView_DeleteAllItems(g_hSpriteList);

	UINT spriteCount = g_vSpriteInfos.size();
	for (size_t i = 0; i < spriteCount; i++)
	{
		SpriteInfo *spriteInfo = &g_vSpriteInfos[i];
		LVITEM item = {};
		item.mask = LVIF_TEXT;
		item.iItem = i;
		item.iSubItem = 0; // 아이템을 처음 추가하므로 0번째 서브아이템을 선택한다.
		item.state;
		item.stateMask;

		char itemText[szMax_Pos] = {};
		item.pszText = itemText;
		ListView_InsertItem(g_hSpriteList, &item); // 아이템 추가

		_itoa_s(spriteInfo->Time, itemText, 10);
		ListView_SetItemText(g_hSpriteList, i, 0, itemText);

		UINT subitemCount = 1;
		for (size_t jj = 0; jj < nMax_SpriteCoordinateCount; jj++)
		{
			_itoa_s(spriteInfo->Coordinates[jj], itemText, 10);
			ListView_SetItemText(g_hSpriteList, i, jj + subitemCount, itemText);
		}
	}
}

void UpdateCollisionList() {
	ListView_DeleteAllItems(g_hCollisionList);
	if (g_SpriteInfoIndex != -1) {

		RECT *pCollisionRect;

		LVITEM item = {};
		item.mask = LVIF_TEXT;
		item.iSubItem = 0; // 아이템을 처음 추가하므로 0번째 서브아이템을 선택한다.
		item.state;
		item.stateMask;
		for (size_t i = 0; i < g_SpriteInfo.CollisionCount; i++)
		{
			item.iItem = i;

			char itemText[szMax_Pos] = {};
			item.pszText = itemText;
			ListView_InsertItem(g_hCollisionList, &item); // 아이템 추가

			pCollisionRect = &g_SpriteInfo.Collisions[i];

			_itoa_s(pCollisionRect->left, itemText, 10);
			ListView_SetItemText(g_hCollisionList, i, 0, itemText);
			_itoa_s(pCollisionRect->top, itemText, 10);
			ListView_SetItemText(g_hCollisionList, i, 1, itemText);
			_itoa_s(pCollisionRect->right, itemText, 10);
			ListView_SetItemText(g_hCollisionList, i, 2, itemText);
			_itoa_s(pCollisionRect->bottom, itemText, 10);
			ListView_SetItemText(g_hCollisionList, i, 3, itemText);
		}
	}
}
