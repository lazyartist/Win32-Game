#include "stdafx.h"
#include "CGameFrameTest.h"
#include "lib.h"

CGameFrameTest::CGameFrameTest()
{
}


CGameFrameTest::~CGameFrameTest()
{
}

void CGameFrameTest::InitImpl() {
	//dlog("InitImpl child");
}

void CGameFrameTest::UpdateLogicImpl() {
	//dlog("UpdateLogicImpl child");

}

void CGameFrameTest::UpdateRenderImpl() {
	//dlog("UpdateRenderImpl child");

	RECT rect = { 300, 300, 400, 400 };
	DrawText(_hdcMem, "Test", 9, &rect, DT_LEFT);
}

void CGameFrameTest::ReleaseImpl() {
	//dlog("ReleaseImpl child");
}