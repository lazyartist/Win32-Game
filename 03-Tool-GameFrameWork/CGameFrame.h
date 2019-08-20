#pragma once
#include "lib.h"

class CGameFrame
{
public:
	CGameFrame();
	~CGameFrame();

	void Init(HWND hWnd, UINT frameTime, WH whClient, bool isFullScreen);
	void Update();
	void UpdateRender();
	void Release();

	virtual void InitImpl();
	virtual void UpdateLogicImpl();
	virtual void UpdateRenderImpl();
	virtual void ReleaseImpl();

	void SetFullScreen(bool isFullScreen);

protected:
	HWND _hWnd;
	HDC _hdc;
	HDC _hdcMem;
	HPEN _hClientAreaPen;

	UINT _fps;
	UINT _frameTime;
	WH _whClientSize;
	DWORD _prevFrameTime;

	UINT _fpsPrevCount;
	UINT _fpsCount;
	DWORD _prevSecondTime;
};

