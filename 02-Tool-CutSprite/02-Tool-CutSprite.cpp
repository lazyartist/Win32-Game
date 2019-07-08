#include "stdafx.h"
#include "02-Tool-CutSprite.h"
#include <windowsx.h> // GET_X_LPARAM
#include <commdlg.h> // GetOpenFileName()
#include <iostream> // sprintf_s()
//#include <math.h> // ceil()
#include "common.h"
#include "CutImage.h"

#define MAX_LOADSTRING 100

POINT g_whMainWndSize = { 800, 500 };
POINT g_whBottomWndSize = { 800, 250 };
POINT g_whRightWndSize = { 0, 0 }; // 리소스에 의해 결정

// 전역 변수:
HINSTANCE g_hInst;                                // 현재 인스턴스입니다.

HDC g_hMemDC;
BITMAP g_bitmapHeader;
bool g_bIsDrag = false;
RECT g_rectDrag = { 10, 10, 100, 100 };

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

//void callback_RightWnd();

//RightWnd g_RightWnd;

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

	//HWND hWnd = CreateWindowW(g_szMainWndClass, g_szMainWndTitle, (WS_OVERLAPPEDWINDOW & ~WS_MAXIMIZEBOX/*최대화 버튼 비활성화*/),
	HWND hWnd = CreateWindowW(g_szMainWndClass, g_szMainWndTitle, (WS_VSCROLL | WS_OVERLAPPEDWINDOW & ~WS_MAXIMIZEBOX/*최대화 버튼 비활성화*/),
		CW_USEDEFAULT, 0, clientRect.right - clientRect.left, clientRect.bottom - clientRect.top, nullptr, nullptr, hInstance, nullptr);

	g_hMainWnd = hWnd;

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
		//HBITMAP hBitmap = (HBITMAP)LoadImage(nullptr, "sprites/castlevania_sm.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
		//HBITMAP hBitmap = (HBITMAP)LoadImage(nullptr, "sprites/test.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
		//HBITMAP hBitmap = (HBITMAP)LoadImage(nullptr, "sprites/test_100.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
		HBITMAP hBitmap = (HBITMAP)LoadImage(nullptr, "sprites/test_100_red.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);

		GetObject(hBitmap, sizeof(BITMAP), &g_bitmapHeader);

		HDC hdc = GetDC(hWnd);

		g_hMemDC = CreateCompatibleDC(hdc);
		SelectObject(g_hMemDC, hBitmap);
		//BitBlt(hdc, 0, 0, g_bitmapHeader.bmWidth, g_bitmapHeader.bmHeight, g_hMemDC, 0, 0, SRCCOPY);

		g_CutImage.Init(g_hMemDC, g_bitmapHeader);

		DeleteObject(hBitmap);
		DeleteDC(hdc);

		// transparent color
		g_BitmapViewInfo.TransparentColor = GetPixel(g_hMemDC, 0, 0);
	}
	break;

	case WM_SETCURSOR:
		if (g_BitmapViewInfo.IsTransparentColorPickMode) {
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
		if (g_bIsDrag) {
			float fMagnification = g_BitmapViewInfo.Magnification;

			g_rectDrag.right = round(GET_X_LPARAM(lParam) / fMagnification);
			g_rectDrag.bottom = round(GET_Y_LPARAM(lParam) / fMagnification);
			InvalidateRect(hWnd, nullptr, true);
			dlog(g_rectDrag);
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

			InvalidateRect(g_hRightWnd, nullptr, true);
		}
		else {
			g_bIsDrag = true;

			// 멀티 모니터에서 부정확한 좌표를 반환하기 때문에 GET_X_LPARAM, GET_Y_LPARAM를 사용
			//rect.left = LOWORD(lParam);
			//rect.top = HIWORD(lParam);

			//float fMagnification = g_BitmapViewInfo.Magnification;

			/*g_rectDrag.left = GET_X_LPARAM(lParam) / fMagnification;
			g_rectDrag.top = GET_Y_LPARAM(lParam) / fMagnification;
			g_rectDrag.right = g_rectDrag.left / fMagnification;
			g_rectDrag.bottom = g_rectDrag.top / fMagnification;*/

			g_rectDrag.left = round(GET_X_LPARAM(lParam));
			g_rectDrag.top = round(GET_Y_LPARAM(lParam));
			g_rectDrag.right = g_rectDrag.left;
			g_rectDrag.bottom = g_rectDrag.top;

			//log(g_rectDrag.left, g_rectDrag.top);

			g_rectDrag = DivideMagnificationTo(g_rectDrag, g_BitmapViewInfo.Magnification);
		}
	}
	break;
	case WM_LBUTTONUP:
	{
		if (g_BitmapViewInfo.IsTransparentColorPickMode) {
			g_BitmapViewInfo.IsTransparentColorPickMode = false;
		}
		else {
			g_bIsDrag = false;

			float fMagnification = g_BitmapViewInfo.Magnification;

			auto x = GET_X_LPARAM(lParam);
			auto y = GET_Y_LPARAM(lParam);
			dlog("up x,y = ", x, y);

			g_rectDrag.right = round(GET_X_LPARAM(lParam) / fMagnification);
			g_rectDrag.bottom = round(GET_Y_LPARAM(lParam) / fMagnification);

			// top과 left가 bottom과 right보다 항상 작도록 수정
			if (g_rectDrag.right < g_rectDrag.left) {
				LONG temp = g_rectDrag.left;
				g_rectDrag.left = g_rectDrag.right;
				g_rectDrag.right = temp;
			}
			if (g_rectDrag.bottom < g_rectDrag.top) {
				LONG temp = g_rectDrag.top;
				g_rectDrag.top = g_rectDrag.bottom;
				g_rectDrag.bottom = temp;
			}

			dlog("up rect. = ", x, y);


			g_rectDrag = g_CutImage.FitToImage(g_rectDrag, g_BitmapViewInfo.TransparentColor);

			InvalidateRect(hWnd, nullptr, true);
		}
	}
	break;
	case WM_PAINT:
	{
		PAINTSTRUCT ps;
		HDC hdc = BeginPaint(hWnd, &ps);

		float fMagnification = g_BitmapViewInfo.Magnification;
		// bitmap
		// 단순 복사
		//BitBlt(hdc, 0, 0, g_bitmapHeader.bmWidth, g_bitmapHeader.bmHeight, g_hMemDC, 0, 0, SRCCOPY);
		// 확대/축소 복사
		//StretchBlt(hdc, 0, 0, g_bitmapHeader.bmWidth * 2, g_bitmapHeader.bmHeight * 2, g_hMemDC, 0, 0, g_bitmapHeader.bmWidth, g_bitmapHeader.bmHeight, SRCCOPY);
		// 확대/축소, 지정색 제거 복사
		TransparentBlt(hdc, 0, 0, g_bitmapHeader.bmWidth * fMagnification, g_bitmapHeader.bmHeight * fMagnification,
			g_hMemDC, 0, 0, g_bitmapHeader.bmWidth, g_bitmapHeader.bmHeight, RGB(255, 0, 0));

		// draw rect
		//SetROP2(hdc, R2_BLACK); // 외곽은 검은색, 내부는 비어있는 사각형
		//SetROP2(hdc, R2_MASKPEN); // 외곽은 검은색, 내부는 비어있는 사각형
		SetROP2(hdc, R2_NOTXORPEN); // 외곽은 반전색, 내부는 비어있는 사각형

		// Rectangle 고려사항
		// 1. Rectangle은 left, top좌표에 해당하는 픽셀에 라인을 그리지만 right, bottom은 해당하는 픽셀 - 1 위치에 라인을 그린다.
		// 따라서 right, bottom에 +1 한다.
		// 2. 확대 시 픽셀이 1x1 -> 2x2 -> 3x3 으로 확대되기 때문에 드래그 영역을 배수곱만 하면 확대된 픽셀의 첫번째 픽셀에 그려진다.
		// 따라서 확대된 픽셀의 바깥쪽에 라인을 그리려면 배수를 더해주는데 2배이상부터 보정해야하므로 배율에서 -1한 값을 더해준다.
		UINT pixelOffset = fMagnification - 1;
		Rectangle(hdc, g_rectDrag.left * fMagnification, g_rectDrag.top * fMagnification, g_rectDrag.right * fMagnification + 1 + pixelOffset, g_rectDrag.bottom * fMagnification + 1 + pixelOffset);

		//dlog("rect", g_rectDrag);
		//_dlog("rect m", 4, g_rectDrag.left * fMagnification, g_rectDrag.top * fMagnification, g_rectDrag.right * fMagnification, g_rectDrag.bottom * fMagnification);

		// clear
		//DeleteObject(SelectObject(hdc, hOldPen));

		EndPaint(hWnd, &ps);

		dlog("WM_PAINT");
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
			g_BitmapViewInfo.Magnification += 1.0f;
			InvalidateRect(g_hMainWnd, nullptr, true);
			InvalidateRect(hDlg, nullptr, true);

			//char cMagnification[9];
			//_itoa_s(g_BitmapViewInfo.Magnification, cMagnification, 9, 10);
			//SetDlgItemText(hDlg, IDC_EDIT1, cMagnification);
		}
		break;
		case IDC_BUTTON2: // 축소
		{
			g_BitmapViewInfo.Magnification -= 1.0f;
			InvalidateRect(g_hMainWnd, nullptr, true);
			InvalidateRect(hDlg, nullptr, true);
		}
		break;
		case IDC_BUTTON3: // 컬러 선택
		{
			g_BitmapViewInfo.IsTransparentColorPickMode = true;

			SetCursor(LoadCursor(nullptr, IDC_HAND));
			return (INT_PTR)TRUE; // 리턴 true하지 않으면 이 프로시저에서 메시지 처리에 실패했다고 생각하고 운영체제가 기본 메시지 처리를 실행한다.
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
	}

	return (INT_PTR)FALSE;
}

//void callback_RightWnd()
//{
//	log("callback_RightWnd");
//}

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