#include "stdafx.h"
#include "CSpriteAnimation.h"
#include "common.h"
#include "lib.h"

CSpriteAnimation::CSpriteAnimation()
{
}

CSpriteAnimation::~CSpriteAnimation()
{
}

void CSpriteAnimation::Init(HDC hdc, BITMAP bitmapHeader)
{
	_hdc = hdc;
	_bitmapHeader = bitmapHeader;
}

RECT CSpriteAnimation::FitToImage(RECT rect, COLORREF transparentColor)
{
	_transparentColor = transparentColor;

	RECT resultRect = rect;
	resultRect = ScanTop(resultRect);
	resultRect = ScanBottom(resultRect);
	resultRect = ScanLeft(resultRect);
	resultRect = ScanRight(resultRect);

	return resultRect;
}

RECT CSpriteAnimation::ScanTop(RECT rect)
{
	WH wh = RectToWH(rect);

	for (size_t i = 0; i < wh.h; i++)
	{
		for (size_t j = 0; j < wh.w; j++)
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

RECT CSpriteAnimation::ScanBottom(RECT rect)
{
	WH wh = RectToWH(rect);

	for (size_t i = 0; i < wh.h; i++)
	{
		for (size_t j = 0; j < wh.w; j++)
		{
			COLORREF color= GetPixel(_hdc, rect.left + j, rect.bottom - i);
			if (color != _transparentColor) {
				rect.bottom = rect.bottom - i;
				return rect;
			}
		}
	}

	return rect;
}


RECT CSpriteAnimation::ScanLeft(RECT rect)
{
	WH wh = RectToWH(rect);

	for (size_t i = 0; i < wh.w; i++)
	{
		for (size_t j = 0; j < wh.h; j++)
		{
			COLORREF color = GetPixel(_hdc, rect.left + i, rect.top + j);
			if (color != _transparentColor) {
				rect.left = rect.left + i;
				return rect;
			}
		}
	}

	return rect;
}

RECT CSpriteAnimation::ScanRight(RECT rect)
{
	WH wh = RectToWH(rect);

	for (size_t i = 0; i < wh.w; i++)
	{
		for (size_t j = 0; j < wh.h; j++)
		{
			COLORREF color = GetPixel(_hdc, rect.right - i, rect.top + j);
			if (color != _transparentColor) {
				rect.right = rect.right - i;
				return rect;
			}
		}
	}

	return rect;
}

