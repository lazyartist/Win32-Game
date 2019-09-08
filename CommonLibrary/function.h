#pragma once

#include <windows.h>
#include <commdlg.h>
//#include "lib.h"

inline bool OpenFileDialog(char *filePath, char *fileTitle = nullptr) {
//bool OpenFileDialog(OPENFILENAME &ofn) {
	// 빈문자열로 만들어야 파일 다이얼로그가 열린다.
	//g_szAniFilePath[0] = 0;

	// static 또는 전역변수로 선언하지 않으면 다이얼로그가 열리지 않음
	//static char lpstrFile[MAX_PATH];
	// static 또는 전역변수로 선언하지 않아도 다이얼로그가 열림(강의에서는 필요하다고 함)
	//char lpstrFileTitle[MAX_PATH] = {};
	//static char lpstrFileTitle[MAX_PATH] = {};

	OPENFILENAME ofn = { 0, };
	ofn.lStructSize = sizeof(OPENFILENAME);
	ofn.hwndOwner = nullptr;
	//ofn.hwndOwner = hDlg;
	ofn.lpstrFilter = "pUnit State Pattern(*.*)\0*.*\0";
	//ofn.lpstrFilter = "pUnit State Pattern(*.usp)\0*.usp\0";
	ofn.lpstrFile = filePath;
	//ofn.lpstrFile = g_szAniFilePath;
	//ofn.lpstrFile = lpstrFile;
	ofn.lpstrFileTitle = fileTitle;
	//ofn.lpstrFileTitle = lpstrFileTitle;
	ofn.nMaxFile = MAX_PATH;
	ofn.nMaxFileTitle = MAX_PATH;
	ofn.lpstrTitle = "title";

	// 이 프로그램에서 사용한 마지막 경로를 기억하고 있다가 기본 폴더로 열어주는데 운영체제가 해주는 듯하다.
	// 특정 폴더를 기본폴더로 지정하려면 GetModuleFileName, GetCurrentDirectory를 활용한다.
	// 현재 실행파일의 경로
	//char defaultPath[MAX_PATH];
	//GetModuleFileName(nullptr, defaultPath, MAX_PATH);

	// 기본 폴더 지정
	//ofn.lpstrInitialDir = defaultPath;
	//ofn.lpstrInitialDir = "C:\\";

	return GetOpenFileName(&ofn);
}