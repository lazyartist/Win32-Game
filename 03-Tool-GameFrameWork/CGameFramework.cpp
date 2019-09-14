#include "stdafx.h"
#include "CGameFrameWork.h"
#include "lib.h"
#include "common.h"


CGameFramework::CGameFramework() {
	dlog("CGameFramework");
	timeBeginPeriod(1);
}
CGameFramework::~CGameFramework() {
	dlog("~CGameFramework");
	timeEndPeriod(1);
}
void CGameFramework::Init(HWND hWnd, HWND hCanvas, UINT frameTime, WH whClientSize, EWindowMode windowMode) {
	_hWnd = hWnd;
	_hCanvas = hCanvas;
	_frameTime = frameTime;
	_whClientSize = whClientSize;

	_prevFrameTime = Func::GetTick();
	_prevSecondTime = Func::GetTick();
	_fps = 0;
	_fpsPrevCount = 0;
	_hdc = GetDC(_hCanvas);
	_hdcMem = CreateCompatibleDC(_hdc);
	_hClientAreaPen = CreatePen(PS_SOLID, 1, RGB(255, 0, 0));
	SetBkMode(_hdcMem, TRANSPARENT);//텍스트 출력시 배경을 투명하게 한다.
	// CreateCompatibleBitmap에 원본 DC인 _hdc를 전달해야한다.
	// _hdcMem은 아주작고 기본적인 비트맵만 있기 때문에 CreateCompatibleBitmap에 _hdcMem을 전달하면 이와 동일한 비트맵을 생성하기 때문이다. 컬러적용도 안된다.
	HBITMAP hBitmap = CreateCompatibleBitmap(_hdc, _whClientSize.w, _whClientSize.h);
	SelectObject(_hdcMem, hBitmap);
	DeleteObject(hBitmap);

	switch (windowMode) {
	case EWindowMode_None:
	{
		// 현재 윈도우를 화면 가운데 위치시킨다.
		RECT clientRect = {};
		GetClientRect(hWnd, &clientRect);
		AdjustWindowRect(&clientRect, WS_OVERLAPPEDWINDOW, false);
		int screenX = GetSystemMetrics(SM_CXFULLSCREEN);
		int screenY = GetSystemMetrics(SM_CYFULLSCREEN);
		int clientX = clientRect.right - clientRect.left;
		int clientY = clientRect.bottom - clientRect.top;
		SetWindowPos(_hWnd, nullptr, screenX / 2 - clientX / 2, screenY / 2 - clientY / 2,
			clientRect.right - clientRect.left, clientRect.bottom - clientRect.top, SWP_SHOWWINDOW);
	}
	break;
	case EWindowMode_Window:
		SetFullScreen(false);
		break;
	case EWindowMode_FullScreen:
		SetFullScreen(true);
		break;
	default:
		break;
	}

	InitImpl();
}
bool CGameFramework::UpdateFrame() {
	DWORD time = Func::GetTick();
	DWORD nDeltaTime = time - _prevFrameTime;
	if (nDeltaTime >= _frameTime) {
		_fps = 1000 / nDeltaTime;
		_prevFrameTime = time;
		// 넘어간 시간만큼 이전 시간 설정시 빼줌(게임엔진들은 이런 방식으로 안하는듯.)
		//_prevFrameTime = time - (elapsedTime - _frameTime);

		_nDeltaTime = nDeltaTime;
		_fDeltaTime = (float)_nDeltaTime / 1000;
		_fpsCount++;

		//if (bPlaying) {
			return true;
		//}
	}

	// test.1
	// imac 2019, i5-8500 기준 1초에 4000번 정도 호출된다.
	// GetTickCount()로 찍어봤을 때 같은 시간이 상당히 많이 찍히고 16밀리초 후에 다시 많이 찍히는 걸로 봐서
	// 대략 1/60초에 한번씩 실행권을 넘겨받는듯 하다.
	// 아니다!! GetTickCount의 해상도가 11~16ms이기 때문이다.
	// timeGetTime()을 사용하면 해결 할 수 있다.
	//_fpsCount++;

	if (time - _prevSecondTime >= 1000) {
		_fpsPrevCount = _fpsCount;

		_fpsCount = 0;
		_prevSecondTime = time;

		// test.1
		//UpdateLogicImpl();
		//UpdateRender();
		//_prevFrameTime = time;
	}

	return false;
}
void CGameFramework::UpdateLogic() {
	if (bPlaying) {
		UpdateLogicImpl();
	}
}
void CGameFramework::UpdateController() {
	if (bPlaying) {
		UpdateControllerImpl();
	}
}
void CGameFramework::UpdateRender() {
	//if (!bPlaying) { return; }
	//dlog("UpdateRender");

	PatBlt(_hdcMem, 0, 0, _whClientSize.w, _whClientSize.h, WHITENESS);

	UpdateRenderImpl();

	// draw fps
	//char szFps[9];
	//_itoa_s(_fps, szFps, nStrLen_FPS, 10);
	//RECT rectFps = { 0, 0, 100, 100 };
	//DrawText(_hdcMem, szFps, strlen(szFps), &rectFps, DT_LEFT);
	//// draw fps count
	//char szFpsPrevCount[nStrLen_FPS];
	//_itoa_s(_fpsPrevCount, szFpsPrevCount, nStrLen_FPS, 10);
	//RECT rectFpsCount = { 0, 100, 100, 200 };
	//DrawText(_hdcMem, szFpsPrevCount, strlen(szFpsPrevCount), &rectFpsCount, DT_LEFT);
	// draw _nDeltaTime
	char szNDeltaTime[nStrLen_FPS];
	_itoa_s(_nDeltaTime, szNDeltaTime, nStrLen_FPS, 10);
	RECT rectNDeltaTime = { _whClientSize.w - 20, 0, _whClientSize.w, 20 };
	DrawText(_hdcMem, szNDeltaTime, strlen(szNDeltaTime), &rectNDeltaTime, DT_LEFT);
	// 클라이언트 영역이 지정한데로 설정됐는지 우하단에 점을 찍어 확인
	SelectObject(_hdcMem, _hClientAreaPen);
	MoveToEx(_hdcMem, _whClientSize.w - 2, _whClientSize.h - 2, nullptr);
	LineTo(_hdcMem, _whClientSize.w, _whClientSize.h);

	// 백버퍼를 프라이머리 버퍼에 복사
	BitBlt(_hdc, 0, 0, _whClientSize.w, _whClientSize.h, _hdcMem, 0, 0, SRCCOPY);
}
void CGameFramework::Release() {
	//dlog("Release");

	ReleaseDC(_hCanvas, _hdc);
	DeleteDC(_hdcMem);
	DeleteObject(_hClientAreaPen);
	ReleaseImpl();
}
void CGameFramework::PlayStop(bool bPlay) {
	bPlaying = bPlay;
}
void CGameFramework::InitImpl() {
	//dlog("InitImpl");
}
void CGameFramework::UpdateLogicImpl() {
	//dlog("UpdateLogicImpl");
}
void CGameFramework::UpdateControllerImpl() {
	//dlog("UpdateControllerImpl");
}
void CGameFramework::UpdateRenderImpl() {
	//dlog("UpdateRenderImpl");
}
void CGameFramework::ReleaseImpl() {
	//dlog("ReleaseImpl");
}
void CGameFramework::SetFullScreen(bool isFullScreen) {
	if (isFullScreen) {
		// 전체화면 스타일로 설정
		SetWindowLong(_hWnd, GWL_STYLE, WS_POPUP);
		SetWindowPos(_hWnd, nullptr, 0, 0, _whClientSize.w, _whClientSize.h, SWP_SHOWWINDOW);

		DEVMODE sDevMode;
		sDevMode.dmSize = sizeof(DEVMODE);
		sDevMode.dmPelsWidth = _whClientSize.w;
		sDevMode.dmPelsHeight = _whClientSize.h;
		sDevMode.dmFields = DM_PELSWIDTH | DM_PELSHEIGHT; // DM_BITSPERPEL이 뭔지는 모르겠지만 넣으면 전체화면이 안됨
		ChangeDisplaySettings(&sDevMode, CDS_FULLSCREEN);
	}
	else {
		// 창 스타일로 설정
		SetWindowLong(_hWnd, GWL_STYLE, WS_OVERLAPPEDWINDOW);

		RECT clientRect = {};
		SetRect(&clientRect, 0, 0, _whClientSize.w - 1, _whClientSize.h - 1);
		AdjustWindowRect(&clientRect, WS_OVERLAPPEDWINDOW, false);
		SetWindowPos(_hWnd, nullptr, 0, 0, clientRect.right - clientRect.left, clientRect.bottom - clientRect.top, SWP_SHOWWINDOW);
	}
}