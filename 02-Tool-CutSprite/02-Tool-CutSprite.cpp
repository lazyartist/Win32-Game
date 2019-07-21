#include "stdafx.h"
#include <windowsx.h> // GET_X_LPARAM
#include <commdlg.h> // GetOpenFileName()
#include <iostream> // sprintf_s()
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

HDC g_hBitmapSrcDC;
HDC g_hBufferMemDC;
HBITMAP g_hBufferBitmap;
//HBITMAP g_hBitmap;
BITMAP g_bitmapHeader;
HWND g_hBoxList = nullptr;

bool g_bIsLBDrag = false;
RECT g_rectLBDrag = { 0, 0, 0, 0 };
POINT g_pntScrollPos = { 0, 0 };
bool g_bIsRBDrag = false;
RECT g_rectRBDrag = { 0, 0, 0, 0 };
POINT g_pntScrollPosWhenRButtonDown = { 0, 0 };

HWND g_hMainWnd;                                  // 메인 윈도우
HWND g_hRightWnd;                                 // 오른쪽 윈도우
HWND g_hBottomWnd;                                // 하단 윈도우

WCHAR g_szMainWndTitle[] = L"Main";               // 제목 표시줄 텍스트
WCHAR g_szBottomWndTitle[] = L"Bottom";           // 제목 표시줄 텍스트

WCHAR g_szMainWndClass[] = L"MainWnd";            // 메인창 클래스 이름
WCHAR g_szBottomWndClass[] = L"BottomWnd";        // 하단창 클래스 이름

const UINT g_nRightDlgId = IDD_DIALOG1;           // 우측 다이얼로그 리소스 ID
const UINT g_nMenuId = IDC_MY02TOOLCUTSPRITE;			  // 메뉴 ID
const UINT g_nIconId = IDI_MY02TOOLCUTSPRITE;			  // 아이콘 ID
const UINT g_nSmallIconId = IDI_SMALL;			  // 작은 아이콘 ID
const UINT g_nMenuId_OpenFile = ID_32771;		  // 파일 열기 메뉴 ID
const UINT g_nMenuId_SaveFile = ID_32772;		  // 파일 저장 메뉴 ID

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
void AddBoxToList(RECT box);
void AddMagnification(float v);
void SaveBoxListToFile();
void LoadBoxListFromFile();

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

	// 스크롤 넓이를 구해서 클라이언트 영역에서 빼주면 스크롤 영역이 딱 맞지가 않는다. 일단 대충 *4해서 맞춘다.
	g_nScrollbarWidth = GetSystemMetrics(SM_CXVSCROLL) * 4;

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
		// load bitmap
		HBITMAP g_hBitmap = (HBITMAP)LoadImage(nullptr, "sprites/castlevania.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
		//g_hBitmap = (HBITMAP)LoadImage(nullptr, "sprites/castlevania_sm.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
		//g_hBitmap = (HBITMAP)LoadImage(nullptr, "sprites/test_100_red.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
		//HBITMAP hBitmap = (HBITMAP)LoadImage(nullptr, "sprites/test.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
		//HBITMAP hBitmap = (HBITMAP)LoadImage(nullptr, "sprites/test_100.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);

		GetObject(g_hBitmap, sizeof(BITMAP), &g_bitmapHeader);

		HDC hdc = GetDC(hWnd);

		// 원본 Bitmap DC
		g_hBitmapSrcDC = CreateCompatibleDC(hdc);
		SelectObject(g_hBitmapSrcDC, g_hBitmap);
		
		// 버퍼 DC
		g_hBufferMemDC = CreateCompatibleDC(hdc);
		// 메모리 DC에는 기본적으로 아주 작은 크기의 비트맵만 있기 때문에 다른 DC를 복사하려면 비트맵을 만들어줘야한다.
		// SetObject()로 비트맵을 지정할 경우는 안해도 된다.(SetObject()가 뭐지?)
		g_hBufferBitmap = CreateCompatibleBitmap(hdc, g_bitmapHeader.bmWidth, g_bitmapHeader.bmHeight);
		SelectObject(g_hBufferMemDC, g_hBufferBitmap);

		g_CutImage.Init(g_hBitmapSrcDC, g_bitmapHeader);

		DeleteObject(g_hBitmap);
		DeleteDC(hdc);

		// transparent color
		g_BitmapViewInfo.TransparentColor = GetPixel(g_hBitmapSrcDC, 0, 0);

		UpdateMainWndScroll();

		SetTimer(hWnd, 1, 1000 / nFrameRate, nullptr);
	}
	break;

	case WM_SETCURSOR:
		dlog("WM_SETCURSOR", g_bIsRBDrag);
		if (g_bIsRBDrag) {
			SetCursor(LoadCursor(nullptr, IDC_SIZEALL));
			return (INT_PTR)TRUE; // 리턴 true하지 않으면 이 프로시저에서 메시지 처리에 실패했다고 생각하고 운영체제가 기본 메시지 처리를 실행한다.
		}
		else if (g_BitmapViewInfo.IsTransparentColorPickMode) {
			SetCursor(LoadCursor(nullptr, IDC_HAND));
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
				int nMaxScrollX = (g_bitmapHeader.bmWidth * g_BitmapViewInfo.Magnification - g_whMainWndSize.x) + g_nScrollbarWidth;
				int nMaxScrollY = (g_bitmapHeader.bmHeight * g_BitmapViewInfo.Magnification - g_whMainWndSize.y) + g_nScrollbarWidth;

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
		if (g_BitmapViewInfo.IsTransparentColorPickMode) {
			int x = GET_X_LPARAM(lParam);
			int y = GET_Y_LPARAM(lParam);

			HDC hdc = GetDC(hWnd);
			COLORREF color = GetPixel(hdc, x, y);
			g_BitmapViewInfo.TransparentColor = color;
			DeleteDC(hdc);

			//InvalidateRect(g_hRightWnd, nullptr, true);
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
		if (g_BitmapViewInfo.IsTransparentColorPickMode) {
			g_BitmapViewInfo.IsTransparentColorPickMode = false;
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

		// bitmap
		// 버퍼 DC를 원본 비트맵 DC로 덮어쓰며 확대/축소한다.
		// 단순 복사(BitBlt), 확대/축소 복사(StretchBlt)
		// 확대/축소, 지정색 제거 복사
		TransparentBlt(g_hBufferMemDC, -g_pntScrollPos.x, -g_pntScrollPos.y, g_bitmapHeader.bmWidth * fMagnification, g_bitmapHeader.bmHeight * fMagnification, g_hBitmapSrcDC, 0, 0, g_bitmapHeader.bmWidth, g_bitmapHeader.bmHeight, RGB(255, 0, 0));

		// draw rect
		//SetROP2(hdc, R2_BLACK); // 외곽은 검은색, 내부는 비어있는 사각형
		SetROP2(g_hBufferMemDC, R2_MASKPEN); // 외곽은 검은색, 내부는 비어있는 사각형

		// Rectangle 고려사항
		// 1. Rectangle은 left, top좌표에 해당하는 픽셀에 라인을 그리지만 right, bottom은 해당하는 픽셀 - 1 위치에 라인을 그린다.
		// 따라서 right, bottom에 +1 한다.
		// 2. 확대 시 픽셀이 1x1 -> 2x2 -> 3x3 으로 확대되기 때문에 드래그 영역을 배수곱만 하면 확대된 픽셀의 첫번째 픽셀에 그려진다.
		// 따라서 확대된 픽셀의 바깥쪽에 라인을 그리려면 배수를 더해주는데 2배이상부터 보정해야하므로 배율에서 -1한 값을 더해준다.
		UINT pixelOffset = fMagnification - 1;
		char szItem[szMax_Pos];
		RECT rectBox;

		HWND hList = GetDlgItem(g_hRightWnd, IDC_LIST1);
		int itemCount = ListView_GetItemCount(hList);
		int selectedListItemIndex = ListView_GetNextItem(
			g_hBoxList, // 윈도우 핸들
			-1, // 검색을 시작할 인덱스
			LVNI_SELECTED // 검색 조건
		);

		HPEN hPen = CreatePen(PS_DASHDOT, 1, RGB(0, 255, 0));
		HPEN hOldPen = nullptr;
		for (size_t i = 0; i < itemCount; i++)
		{
			ListView_GetItemText(hList, i, 1, szItem, szMax_Pos);
			rectBox.left = atoi(szItem);
			ListView_GetItemText(hList, i, 2, szItem, szMax_Pos);
			rectBox.top = atoi(szItem);
			ListView_GetItemText(hList, i, 3, szItem, szMax_Pos);
			rectBox.right = atoi(szItem);
			ListView_GetItemText(hList, i, 4, szItem, szMax_Pos);
			rectBox.bottom = atoi(szItem);


			if (selectedListItemIndex == i) {
				hOldPen = (HPEN)SelectObject(g_hBufferMemDC, hPen);
				//SetROP2(hdc, R2_MASKPEN); // 외곽은 검은색, 내부는 비어있는 사각형
				SetROP2(g_hBufferMemDC, R2_MERGENOTPEN); // 외곽은 핑크, 내부는 비어있는 사각형
			}
			else {
				SetROP2(g_hBufferMemDC, R2_MASKPEN); // 외곽은 검은색, 내부는 비어있는 사각형
			}

			rectBox = rectBox * fMagnification;
			rectBox = rectBox - g_pntScrollPos;
			Rectangle(g_hBufferMemDC, rectBox.left, rectBox.top, rectBox.right + 1 + pixelOffset, rectBox.bottom + 1 + pixelOffset);

			if (hOldPen != nullptr) {
				(HPEN)SelectObject(g_hBufferMemDC, hOldPen);
				hOldPen = nullptr;
			}
		}
		DeleteObject(hPen);

		// 현재 드래그 영역
		if (g_rectLBDrag.left != g_rectLBDrag.right) {
			SetROP2(g_hBufferMemDC, R2_NOTXORPEN); // 외곽은 반전색, 내부는 비어있는 사각형
			RECT rectDrag = g_rectLBDrag * fMagnification;
			rectDrag = rectDrag - g_pntScrollPos;
			Rectangle(g_hBufferMemDC, rectDrag.left, rectDrag.top, rectDrag.right + 1 + pixelOffset, rectDrag.bottom + 1 + pixelOffset);
		}

		// 버퍼 DC를 그려준 뒤 WM_PAINT 메시지 발생을 위해 InvalidateRect()호출한다.
		// 이때 버퍼DC 전체를 복사하므로 화면을 지울 필요가 없기 때문에 세번째 인자는 false를 전달한다.
		InvalidateRect(hWnd, nullptr, false);
	}
	break;

	case WM_PAINT:
	{
		PAINTSTRUCT ps;
		HDC hdc = BeginPaint(hWnd, &ps);

		BitBlt(hdc, 0, 0, g_bitmapHeader.bmWidth, g_bitmapHeader.bmHeight, g_hBufferMemDC, 0, 0, SRCCOPY);

		// clear
		//DeleteObject(SelectObject(hdc, hOldPen));

		EndPaint(hWnd, &ps);

		dlog("WM_PAINT aa");
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

			// ScrollWindow를 호출하지 않을 경우 InvalidateRect를 호출해야한다.
			//InvalidateRect(hWnd, nullptr, true);
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

		// Box list
		g_hBoxList = GetDlgItem(hDlg, IDC_LIST1);

		// ===== List에 컬럼 추가 =====
		// List 속성의 View를 Report로 설정해야 컬럼을 추가할 수 있다.
		char colText0[] = "No";
		char colText1[] = "left";
		char colText2[] = "top";
		char colText3[] = "right";
		char colText4[] = "bottom";

		LVCOLUMN col = {};
		col.mask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM;
		col.fmt = LVCFMT_LEFT;
		col.cx = 50;
		col.pszText = colText0;
		ListView_InsertColumn(g_hBoxList, 0, &col); // 컬럼 추가0

		col.pszText = colText1;
		ListView_InsertColumn(g_hBoxList, 1, &col); // 컬럼 추가1

		col.pszText = colText2;
		ListView_InsertColumn(g_hBoxList, 2, &col); // 컬럼 추가2

		col.pszText = colText3;
		ListView_InsertColumn(g_hBoxList, 3, &col); // 컬럼 추가3

		col.pszText = colText4;
		ListView_InsertColumn(g_hBoxList, 3, &col); // 컬럼 추가4
		// ===== List에 컬럼 추가 ===== end

		LoadBoxListFromFile();

		// ===== List 뷰 설정 =====
		// 기본값은 첫 번째 서브아이템의 텍스트 영역만 선택됨
		ListView_SetExtendedListViewStyle(
			g_hBoxList,
			LVS_EX_FULLROWSELECT // 아이템 전체가 클릭되도록 한다.
			| LVS_EX_GRIDLINES // 서브아이템 사이에 그리드 라인을 넣는다.
		);
		// ===== List 뷰 설정 ===== end
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
		if (g_BitmapViewInfo.IsTransparentColorPickMode) {
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
			g_BitmapViewInfo.IsTransparentColorPickMode = true;

			SetCursor(LoadCursor(nullptr, IDC_HAND));
			//return (INT_PTR)TRUE; // 리턴 true하지 않으면 이 프로시저에서 메시지 처리에 실패했다고 생각하고 운영체제가 기본 메시지 처리를 실행한다.
		}
		break;
		case IDC_BUTTON4: // 등록
		{
			AddBoxToList(g_rectLBDrag);
			//return (INT_PTR)TRUE; // 리턴 true하지 않으면 이 프로시저에서 메시지 처리에 실패했다고 생각하고 운영체제가 기본 메시지 처리를 실행한다.
		}
		break;
		case IDC_BUTTON5: // 삭제
		{
			int selectedIndex = ListView_GetNextItem(
				g_hBoxList, // 윈도우 핸들
				-1, // 검색을 시작할 인덱스
				LVNI_SELECTED // 검색 조건
			);

			if (selectedIndex < 0) break;

			ListView_DeleteItem(g_hBoxList, selectedIndex);

			//InvalidateRect(g_hMainWnd, nullptr, true);
			//return (INT_PTR)TRUE; // 리턴 true하지 않으면 이 프로시저에서 메시지 처리에 실패했다고 생각하고 운영체제가 기본 메시지 처리를 실행한다.
		}
		break;
		case IDC_BUTTON6: // 파일 저장
		{
			SaveBoxListToFile();
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
				UINT itemIndex = ListView_GetNextItem(
					pnmhdr->hwndFrom, // 윈도우 핸들
					-1, // 검색을 시작할 인덱스
					LVNI_SELECTED // 검색 조건
				);
			}
			else if (pnmhdr->code == LVN_ITEMCHANGED) {
				//InvalidateRect(g_hMainWnd, nullptr, true);
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
	int wBitmap = g_bitmapHeader.bmWidth * g_BitmapViewInfo.Magnification;
	int hBitmap = g_bitmapHeader.bmHeight * g_BitmapViewInfo.Magnification;
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

void AddBoxToList(RECT box) {
	int itemCount = ListView_GetItemCount(g_hBoxList);

	// ===== List에 아이템 추가 =====
	char itemText0[szMax_Pos] = {};
	char itemText1[szMax_Pos] = {};
	char itemText2[szMax_Pos] = {};
	char itemText3[szMax_Pos] = {};
	char itemText4[szMax_Pos] = {};

	_itoa_s(itemCount, itemText0, szMax_Pos, 10);
	_itoa_s(g_rectLBDrag.left, itemText1, szMax_Pos, 10);
	_itoa_s(g_rectLBDrag.top, itemText2, szMax_Pos, 10);
	_itoa_s(g_rectLBDrag.right, itemText3, szMax_Pos, 10);
	_itoa_s(g_rectLBDrag.bottom, itemText4, szMax_Pos, 10);

	//char itemText1[] = "item1";
	LVITEM item = {};
	item.mask = LVIF_TEXT;
	item.iItem = 0;
	item.iSubItem = 0; // 아이템을 처음 추가하므로 0번째 서브아이템을 선택한다.
	item.state;
	item.stateMask;

	// 아이템 추가
	item.pszText = itemText0;
	ListView_InsertItem(g_hBoxList, &item); // 아이템 추가0
	ListView_SetItemText(g_hBoxList, 0/*item idx*/, 1/*subitem idx*/, itemText1); // 서브아이템 추가0
	ListView_SetItemText(g_hBoxList, 0/*item idx*/, 2/*subitem idx*/, itemText2); // 서브아이템 추가0
	ListView_SetItemText(g_hBoxList, 0/*item idx*/, 3/*subitem idx*/, itemText3); // 서브아이템 추가0
	ListView_SetItemText(g_hBoxList, 0/*item idx*/, 4/*subitem idx*/, itemText4); // 서브아이템 추가0
}

void AddMagnification(float v) {
	g_BitmapViewInfo.Magnification += v;

	//InvalidateRect(g_hMainWnd, nullptr, true);
	InvalidateRect(g_hRightWnd, nullptr, true);

	UpdateMainWndScroll();
}

void LoadBoxListFromFile() {
	FILE *file = nullptr;
	file = _fsopen("spritesList.cut", "rt", _SH_DENYNO);

	if (file == nullptr) return;

	HWND hList = GetDlgItem(g_hRightWnd, IDC_LIST1);

	char szItemCount[szMax_Boxes] = {};
	fgets(szItemCount, szMax_Boxes, file);

	// item count
	int itemCount = atoi(szItemCount);

	// ===== List에 아이템 추가 =====
	char itemLine[szMax_PosLine] = {};
	char itemText0[szMax_Pos] = {};
	char itemText1[szMax_Pos] = {};
	char itemText2[szMax_Pos] = {};
	char itemText3[szMax_Pos] = {};
	char itemText4[szMax_Pos] = {};

	char *token;
	char *nextToken;

	for (size_t i = 0; i < itemCount; i++)
	{
		_itoa_s(i, itemText0, 10);

		memset(itemLine, 0, szMax_PosLine);
		fgets(itemLine, szMax_PosLine, file);

		// \n은 줄바꿈을 지정하는 문자이므로 순수 문자만 얻기 위해 제거한다.
		itemLine[strcspn(itemLine, "\n")] = 0; // strcspn()으로 "\n"의 위치를 찾고 그 위치에 0을 넣어준다.

		if (strnlen_s(itemLine, szMax_PosLine) == 0) continue;

		token = strtok_s(itemLine, "\t", &nextToken);
		strcpy_s(itemText1, token);
		token = strtok_s(NULL, "\t", &nextToken);
		strcpy_s(itemText2, token);
		token = strtok_s(NULL, "\t", &nextToken);
		strcpy_s(itemText3, token);
		token = strtok_s(NULL, "\t", &nextToken);
		strcpy_s(itemText4, token);

		//char itemText1[] = "item1";
		LVITEM item = {};
		item.mask = LVIF_TEXT;
		item.iItem = i;
		item.iSubItem = 0; // 아이템을 처음 추가하므로 0번째 서브아이템을 선택한다.
		item.state;
		item.stateMask;

		// 아이템 추가
		item.pszText = itemText0;
		ListView_InsertItem(g_hBoxList, &item); // 아이템 추가0
		ListView_SetItemText(g_hBoxList, i/*item idx*/, 1/*subitem idx*/, itemText1); // 서브아이템 추가0
		ListView_SetItemText(g_hBoxList, i/*item idx*/, 2/*subitem idx*/, itemText2); // 서브아이템 추가0
		ListView_SetItemText(g_hBoxList, i/*item idx*/, 3/*subitem idx*/, itemText3); // 서브아이템 추가0
		ListView_SetItemText(g_hBoxList, i/*item idx*/, 4/*subitem idx*/, itemText4); // 서브아이템 추가0
	}

	fclose(file);
}

void SaveBoxListToFile() {
	FILE *file = nullptr;
	file = _fsopen("spritesList.cut", "wt", _SH_DENYNO);

	if (file == nullptr) return;

	HWND hList = GetDlgItem(g_hRightWnd, IDC_LIST1);

	// item count
	int itemCount = ListView_GetItemCount(hList);
	char szItemCount[szMax_Pos] = {};
	sprintf_s<szMax_Pos>(szItemCount, "%d\n", itemCount);
	fputs(szItemCount, file);

	// items
	char szLeft[szMax_Pos];
	char szTop[szMax_Pos];
	char szRight[szMax_Pos];
	char szBottom[szMax_Pos];
	for (size_t i = 0; i < itemCount; i++)
	{
		ListView_GetItemText(hList, i, 1, szLeft, szMax_Pos);
		ListView_GetItemText(hList, i, 2, szTop, szMax_Pos);
		ListView_GetItemText(hList, i, 3, szRight, szMax_Pos);
		ListView_GetItemText(hList, i, 4, szBottom, szMax_Pos);

		fprintf_s(file, "%s\t%s\t%s\t%s\n", szLeft, szTop, szRight, szBottom);
	}

	fclose(file);
}