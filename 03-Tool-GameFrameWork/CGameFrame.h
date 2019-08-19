#pragma once
class CGameFrame
{
public:
	CGameFrame();
	~CGameFrame();

	void Init(HWND hWnd, UINT frameTime);
	void Update();
	void UpdateRender();
	void Release();

	virtual void InitImpl();
	virtual void UpdateLogicImpl();
	virtual void UpdateRenderImpl();
	virtual void ReleaseImpl();

private:
	HWND _hWnd;

	UINT _fps;
	UINT _frameTime;
	DWORD _prevFrameTime;

	UINT _fpsPrevCount;
	UINT _fpsCount;
	DWORD _prevSecondTime;
};

