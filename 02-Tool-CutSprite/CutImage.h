#pragma once

#include "stdafx.h"

class CutImage
{
public:
	CutImage();
	~CutImage();

	void Init(HDC hdc, BITMAP bitmapHeader);
	RECT FitToImage(RECT rect, COLORREF transparentColor);

private:
	RECT ScanTop(RECT rect);

	HDC _hdc;
	COLORREF _transparentColor;
	BITMAP _bitmapHeader;

	//RECT _rectFit;
	//POINT _ptStart;
};

