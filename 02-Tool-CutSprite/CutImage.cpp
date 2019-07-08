#include "stdafx.h"
#include "CutImage.h"

CutImage::CutImage()
{
}

CutImage::~CutImage()
{
}

void CutImage::Init(HDC hdc, BITMAP bitmapHeader)
{
	_hdc = hdc;
	_bitmapHeader = bitmapHeader;
}

RECT CutImage::FitToImage(RECT rect, COLORREF transparentColor)
{
	_transparentColor = transparentColor;

	RECT resultRect;

	//_ptStart = ptStart;
	resultRect = ScanTop(rect);

	return resultRect;
}

RECT CutImage::ScanTop(RECT rect)
{
	for (size_t i = 0; i < _bitmapHeader.bmHeight; i++)
	{
		for (size_t j = 0; j < _bitmapHeader.bmWidth; j++)
		{
			COLORREF color= GetPixel(_hdc, rect.left + j, rect.top + i);
			if (color != _transparentColor) {
				rect.top = rect.top + i;
				return rect;
			}
		}
	}

	return rect;
}
