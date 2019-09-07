#pragma once

#include "stdafx.h"

class CSpriteAnimation {
public:
	CSpriteAnimation();
	~CSpriteAnimation();

	void Init(HDC hdc, BITMAP bitmapHeader);
	RECT FitToImage(RECT rect, COLORREF transparentColor);

private:
	HDC _hdc;
	COLORREF _transparentColor;
	BITMAP _bitmapHeader;

	RECT ScanTop(RECT rect);
	RECT ScanBottom(RECT rect);
	RECT ScanLeft(RECT rect);
	RECT ScanRight(RECT rect);
};

