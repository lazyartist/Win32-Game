#pragma once
#include "lib.h"

class CGameFrame
{
public:
	CGameFrame();
	virtual ~CGameFrame();

	void Init(HWND hWnd, HWND hCanvas, UINT frameTime, WH whClient, EWindowMode windowMode);
	void Update();
	void UpdateRender();
	void Release();

	// implement derived class
	virtual void InitImpl();
	virtual void UpdateLogicImpl();
	virtual void UpdateRenderImpl();
	virtual void ReleaseImpl();

	void SetFullScreen(bool isFullScreen);

protected:
	HWND _hWnd;
	HWND _hCanvas;
	HDC _hdc;
	HDC _hdcMem;
	HPEN _hClientAreaPen;

	UINT _fps;
	UINT _frameTime;
	WH _whClientSize;
	DWORD _prevFrameTime;
	UINT _nDeltaTime;
	FLOAT _fDeltaTime;

	UINT _fpsPrevCount;
	UINT _fpsCount;
	DWORD _prevSecondTime;
};

