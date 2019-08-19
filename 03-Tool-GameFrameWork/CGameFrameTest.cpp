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
	//CGameFrame::InitImpl();
	dlog("InitImpl child");
}

void CGameFrameTest::UpdateImpl() {
	//CGameFrame::InitImpl();
	dlog("UpdateImpl child");
}

void CGameFrameTest::RenderImpl() {
	//CGameFrame::InitImpl();
	dlog("RenderImpl child");
}

void CGameFrameTest::ReleaseImpl() {
	//CGameFrame::InitImpl();
	dlog("ReleaseImpl child");
}