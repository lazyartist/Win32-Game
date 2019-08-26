#include "stdafx.h"
#include "CGameFrame.h"
#include "lib.h"
#include "common.h"


CGameFrame::CGameFrame()
{
	dlog("CGameFrame");
}

CGameFrame::~CGameFrame()
{
	dlog("~CGameFrame");
}

void CGameFrame::Init(HWND hWnd, HWND hCanvas, UINT frameTime, WH whClientSize, WindowMode windowMode)
{
	dlog("Init");

	_hWnd = hWnd;
	_hCanvas = hCanvas;
	_frameTime = frameTime;
	_whClientSize = whClientSize;

	_prevFrameTime = GetTickCount();
	_prevSecondTime = GetTickCount();

	_fps = 0;
	_fpsPrevCount = 0;

	_hdc = GetDC(_hCanvas);
	_hdcMem = CreateCompatibleDC(_hdc);
	// CreateCompatibleBitmap�� ���� DC�� _hdc�� �����ؾ��Ѵ�.
	// _hdcMem�� �����۰� �⺻���� ��Ʈ�ʸ� �ֱ� ������ CreateCompatibleBitmap�� _hdcMem�� �����ϸ� �̿� ������ ��Ʈ���� �����ϱ� �����̴�. �÷����뵵 �ȵȴ�.
	HBITMAP hBitmap = CreateCompatibleBitmap(_hdc, _whClientSize.w, _whClientSize.h);
	SelectObject(_hdcMem, hBitmap);
	DeleteObject(hBitmap);

	_hClientAreaPen = CreatePen(PS_SOLID, 1, RGB(255, 0, 0));

	switch (windowMode)
	{
	case None:
		break;
	case Window:
		SetFullScreen(false);
		break;
	case FullScreen:
		SetFullScreen(true);
		break;
	default:
		break;
	}

	InitImpl();
}

void CGameFrame::Update()
{
	DWORD time = GetTickCount();
	DWORD nDeltaTime = time - _prevFrameTime;
	if (nDeltaTime >= _frameTime) {
		_fps = 1000 / nDeltaTime;
		_prevFrameTime = time;
		// �Ѿ �ð���ŭ ���� �ð� ������ ����(���ӿ������� �̷� ������� ���ϴµ�.)
		//_prevFrameTime = time - (elapsedTime - _frameTime);

		_nDeltaTime = nDeltaTime;
		_fDeltaTime = (float)_nDeltaTime / 1000;

		_fpsCount++;

		UpdateLogicImpl();
		UpdateRender();
	}

	// test.1
	// imac 2019, i5-8500 ���� 1�ʿ� 4000�� ���� ȣ��ȴ�.
	// GetTickCount()�� ������ �� ���� �ð��� ����� ���� ������ 16�и��� �Ŀ� �ٽ� ���� ������ �ɷ� ����
	// �뷫 1/60�ʿ� �ѹ��� ������� �Ѱܹ޴µ� �ϴ�.
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
}

void CGameFrame::UpdateRender()
{
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

	//// draw _nDeltaTime
	//char szNDeltaTime[nStrLen_FPS];
	//_itoa_s(_nDeltaTime, szNDeltaTime, nStrLen_FPS, 10);
	//RECT rectNDeltaTime = { 0, 200, 100, 300 };
	//DrawText(_hdcMem, szNDeltaTime, strlen(szNDeltaTime), &rectNDeltaTime, DT_LEFT);

	// Ŭ���̾�Ʈ ������ �����ѵ��� �����ƴ��� ���ϴܿ� ���� ��� Ȯ��
	SelectObject(_hdcMem, _hClientAreaPen);
	MoveToEx(_hdcMem, _whClientSize.w - 2, _whClientSize.h - 2, nullptr);
	LineTo(_hdcMem, _whClientSize.w, _whClientSize.h);

	// ����۸� �����̸Ӹ� ���ۿ� ����
	BitBlt(_hdc, 0, 0, _whClientSize.w, _whClientSize.h, _hdcMem, 0, 0, SRCCOPY);
}

void CGameFrame::Release()
{
	//dlog("Release");

	ReleaseDC(_hCanvas, _hdc);
	DeleteDC(_hdcMem);
	DeleteObject(_hClientAreaPen);
	ReleaseImpl();
}

void CGameFrame::InitImpl()
{
	dlog("InitImpl");
}

void CGameFrame::UpdateLogicImpl()
{
	//dlog("UpdateLogicImpl");
}

void CGameFrame::UpdateRenderImpl()
{
	//dlog("UpdateRenderImpl");
}

void CGameFrame::ReleaseImpl()
{
	dlog("ReleaseImpl");
}

void CGameFrame::SetFullScreen(bool isFullScreen) {
	if (isFullScreen) {
		// ��üȭ�� ��Ÿ�Ϸ� ����
		SetWindowLong(_hWnd, GWL_STYLE, WS_POPUP);
		SetWindowPos(_hWnd, nullptr, 0, 0, _whClientSize.w, _whClientSize.h, SWP_SHOWWINDOW);

		DEVMODE sDevMode;
		sDevMode.dmSize = sizeof(DEVMODE);
		sDevMode.dmPelsWidth = _whClientSize.w;
		sDevMode.dmPelsHeight = _whClientSize.h;
		sDevMode.dmFields = DM_PELSWIDTH | DM_PELSHEIGHT; // DM_BITSPERPEL�� ������ �𸣰����� ������ ��üȭ���� �ȵ�
		ChangeDisplaySettings(&sDevMode, CDS_FULLSCREEN);
	}
	else {
		// â ��Ÿ�Ϸ� ����
		SetWindowLong(_hWnd, GWL_STYLE, WS_OVERLAPPEDWINDOW);

		RECT clientRect = {};
		SetRect(&clientRect, 0, 0, _whClientSize.w - 1, _whClientSize.h - 1);
		AdjustWindowRect(&clientRect, WS_OVERLAPPEDWINDOW, false);
		SetWindowPos(_hWnd, nullptr, 0, 0, clientRect.right - clientRect.left, clientRect.bottom - clientRect.top, SWP_SHOWWINDOW);
	}
}