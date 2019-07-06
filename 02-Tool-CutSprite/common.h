#pragma once

#include <iostream> // sprintf_s()
#include "Windows.h"
//#include <vector>

using namespace std;


// ===== define =====
//#define Max_Account_Text 10 + 1
//
//#define Max_Student_Id 6 + 1
//#define Max_Student_Name 10 + 1
//#define Max_Student_Info_Line (Max_Student_Id + 1 + Max_Student_Name)
//
//#define Max_Score_Text 9 + 1
//#define Max_Score_Info_Line (Max_Student_Id + 1 + Max_Score_Text + 1 + Max_Score_Text + 1 + Max_Score_Text)
//
//#define Max_Semester 4
//
//#define Str_ScoreFile_Path_Format "scores/%s_%d.txt"

// ===== define ===== end


// ===== enum =====
//enum LoginResultType
//{
//	Success, Fail
//};
// ===== enum =====  end


// ===== struct =====
//typedef struct _Student {
//	char Id[Max_Student_Id];
//	char Name[Max_Student_Name];
//} Student, *PStudent;
//
//typedef struct _Score {
//	char Course[Max_Score_Text];
//	char Point[Max_Score_Text];
//
//	_Score(const char *course, const char *point) {
//		strcpy_s(Course, course);
//		strcpy_s(Point, point);
//	}
//} Score, *PScore;
// ===== struct ===== end


// ===== function =====
inline void log(LPCSTR lpStr) {
	OutputDebugString(lpStr);
	OutputDebugString("\n");
};

inline void log(LPCSTR lpStr, LPCSTR lpStr2) {
	OutputDebugString(lpStr);
	OutputDebugString(", ");
	OutputDebugString(lpStr2);
	OutputDebugString("\n");
};

inline void log(LPCSTR lpStr, LPCSTR lpStr2, LPCSTR lpStr3) {
	OutputDebugString(lpStr);
	OutputDebugString(", ");
	OutputDebugString(lpStr2);
	OutputDebugString(", ");
	OutputDebugString(lpStr3);
	OutputDebugString("\n");
};

inline void log(int i) {
	char buffer[99];
	sprintf_s(buffer, 99, "%d", i);
	OutputDebugString(buffer);
	OutputDebugString("\n");
};
// ===== function ===== end
