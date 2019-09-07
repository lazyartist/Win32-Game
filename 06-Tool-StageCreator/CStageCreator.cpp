#include "stdafx.h"
#include "CStageCreator.h"
#include "CGameFramework.cpp"

CStageCreator::CStageCreator() {
}
CStageCreator::~CStageCreator() {
}
void CStageCreator::InitImpl() {
	dlog("InitImpl");
}
void CStageCreator::UpdateLogicImpl() {
	dlog("UpdateLogicImpl");
}
void CStageCreator::UpdateControllerImpl() {
	dlog("UpdateControllerImpl");
}
void CStageCreator::UpdateRenderImpl() {
	dlog("UpdateRenderImpl");
}
void CStageCreator::ReleaseImpl() {
	dlog("ReleaseImpl");
}