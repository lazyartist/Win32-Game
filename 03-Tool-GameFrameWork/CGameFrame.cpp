#include "stdafx.h"
#include "CGameFrame.h"
#include "lib.h"


CGameFrame::CGameFrame()
{
	dlog("CGameFrame");
}

CGameFrame::~CGameFrame()
{
	dlog("~CGameFrame");
}

void CGameFrame::Init(HWND hWnd, UINT frameTime)
{
	dlog("Init");

	_hWnd = hWnd;
	_frameTime = frameTime;

	_prevFrameTime = GetTickCount();
	_prevSecondTime = GetTickCount();

	_fps = 0;
	_fpsPrevCount = 0;

	InitImpl();
}

void CGameFrame::Update()
{
	//dlog("Update");

	DWORD time = GetTickCount();
	DWORD elapsedTime = time - _prevFrameTime;
	//dlog("elapsedTime", elapsedTime);
	if (elapsedTime >= _frameTime) {
		//if (elapsedTime == 0) elapsedTime = 1000;
		_fps = 1000 / elapsedTime;
		_prevFrameTime = time;

		_fpsCount++;

		UpdateLogicImpl();
		UpdateRender();
	}

	if (time - _prevSecondTime >= 1000) {
		_fpsPrevCount = _fpsCount;

		_fpsCount = 0;
		_prevSecondTime = time;
	}
}

void CGameFrame::UpdateRender()
{
	//dlog("UpdateRender");

	HDC hdc = GetDC(_hWnd);

	// draw fps
	char szFps[9];
	_itoa_s(_fps, szFps, 9, 10);
	RECT rectFps = { 0, 0, 100, 100 };
	DrawText(hdc, szFps, 9, &rectFps, DT_LEFT);
	
	// draw fps count
	char szFpsPrevCount[9];
	_itoa_s(_fpsPrevCount, szFpsPrevCount, 9, 10);
	RECT rectFpsCount = { 0, 100, 200, 200 };
	DrawText(hdc, szFpsPrevCount, 9, &rectFpsCount, DT_LEFT);

	ReleaseDC(_hWnd, hdc);
	InvalidateRect(_hWnd, nullptr, false);

	//dlog(_time);

	UpdateRenderImpl();
}

void CGameFrame::Release()
{
	//dlog("Release");
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
