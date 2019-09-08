#pragma once

#include <windows.h>
#include <commdlg.h>
//#include "lib.h"

inline bool OpenFileDialog(char *filePath, char *fileTitle = nullptr) {
//bool OpenFileDialog(OPENFILENAME &ofn) {
	// ���ڿ��� ������ ���� ���̾�αװ� ������.
	//g_szAniFilePath[0] = 0;

	// static �Ǵ� ���������� �������� ������ ���̾�αװ� ������ ����
	//static char lpstrFile[MAX_PATH];
	// static �Ǵ� ���������� �������� �ʾƵ� ���̾�αװ� ����(���ǿ����� �ʿ��ϴٰ� ��)
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

	// �� ���α׷����� ����� ������ ��θ� ����ϰ� �ִٰ� �⺻ ������ �����ִµ� �ü���� ���ִ� ���ϴ�.
	// Ư�� ������ �⺻������ �����Ϸ��� GetModuleFileName, GetCurrentDirectory�� Ȱ���Ѵ�.
	// ���� ���������� ���
	//char defaultPath[MAX_PATH];
	//GetModuleFileName(nullptr, defaultPath, MAX_PATH);

	// �⺻ ���� ����
	//ofn.lpstrInitialDir = defaultPath;
	//ofn.lpstrInitialDir = "C:\\";

	return GetOpenFileName(&ofn);
}