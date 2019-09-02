#pragma once
#include "lib.h"

class CGameFrameWorkBase
{
public:
	CGameFrameWorkBase();
	virtual ~CGameFrameWorkBase();

	void Init(HWND hWnd, HWND hCanvas, UINT frameTime, WH whClient, EWindowMode windowMode);
	bool UpdateFrame();
	void UpdateLogic();
	void UpdateController();
	void UpdateRender();
	void Release();
	void PlayStop(bool bPlay);

	// implement derived class
	virtual void InitImpl();
	virtual void UpdateLogicImpl();
	virtual void UpdateControllerImpl();
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
	bool _bPlaying;
};

