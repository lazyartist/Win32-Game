#include "stdafx.h"
#include "CGameFrameworkTest.h"
#include "lib.h"

CGameFrameworkTest::CGameFrameworkTest()
{
}

CGameFrameworkTest::~CGameFrameworkTest()
{
}

void CGameFrameworkTest::InitImpl() {
	//dlog("InitImpl child");
}

void CGameFrameworkTest::UpdateLogicImpl() {
	//dlog("UpdateLogicImpl child");

}

void CGameFrameworkTest::UpdateRenderImpl() {
	//dlog("UpdateRenderImpl child");

	RECT rect = { 300, 300, 400, 400 };
	DrawText(_hdcMem, "Test", 9, &rect, DT_LEFT);
}

void CGameFrameworkTest::ReleaseImpl() {
	//dlog("ReleaseImpl child");
}