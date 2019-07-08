#include "stdafx.h"
#include "CutImage.h"
#include "common.h"

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

	RECT resultRect = rect;
	resultRect = ScanTop(resultRect);
	resultRect = ScanBottom(resultRect);
	resultRect = ScanLeft(resultRect);
	resultRect = ScanRight(resultRect);

	return resultRect;
}

RECT CutImage::ScanTop(RECT rect)
{
	WH wh = ScreenRectToWH(rect);

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

RECT CutImage::ScanBottom(RECT rect)
{
	WH wh = ScreenRectToWH(rect);

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


RECT CutImage::ScanLeft(RECT rect)
{
	WH wh = ScreenRectToWH(rect);

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

RECT CutImage::ScanRight(RECT rect)
{
	WH wh = ScreenRectToWH(rect);

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

