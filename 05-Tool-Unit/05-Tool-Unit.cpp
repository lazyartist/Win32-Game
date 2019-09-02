#include "stdafx.h"
#include "05-Tool-Unit.h"
#include "CUnitCreatorGameFrameWork.h"
#include "lib.h"
#include "Commctrl.h"

HINSTANCE g_hInstance;
const char *g_szUnitStateTypeAsString[] = { "Idle" , "EUnitStateType_MoveTo" };
HWND g_hWnd;
HWND g_hUnitStateAniList;
CUnitCreatorGameFrameWork g_cUnitCreator;
char g_szCurDir[MAX_PATH] = {}; // 작업 경로, 프로그램 실행 중 파일 대화상자에서 선택한 곳으로 바뀌기 때문에 프로그램 실행과 동시에 저장해둔다.
char g_szUnitFilePath[MAX_PATH];

BOOL InitInstance(HINSTANCE, int);
INT_PTR CALLBACK DlgProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
void LoadSettings(const char *filePath);
void SaveSettings(const char *filePath);
void LoadUnit(const char *filePath);
void SaveUnit(const char *filePath);
void UpdateUI();
void UpdateBitmap();
void UpdateUnitStateAniList();

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPWSTR    lpCmdLine,
	_In_ int       nCmdShow) {
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

	if (!InitInstance(hInstance, nCmdShow)) {
		return FALSE;
	}

	HWND hWndPicture = GetDlgItem(g_hWnd, IDC_PIC1);
	g_cUnitCreator.Init(g_hWnd, hWndPicture, 1000 / 90, { 800, 600 }, EWindowMode::None);

	LoadSettings("settings.cfg");
	LoadUnit(g_szUnitFilePath);

	g_cUnitCreator.PlayStop(true);

	MSG msg;
	while (true) {
		if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)) {
			if (msg.message == WM_QUIT) {
				break;
			}
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		if (g_cUnitCreator.Update()) {};
	}
	g_cUnitCreator.Release();

	return (int)msg.wParam;
}
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow) {
	g_hInstance = hInstance; // 인스턴스 핸들을 전역 변수에 저장합니다.
	// 프로그램의 작업 경로 저장
	GetCurrentDirectory(MAX_PATH, g_szCurDir);
	// 다이얼로그 윈도우 생성
	HWND hWnd = CreateDialog(hInstance, MAKEINTRESOURCE(IDD_DIALOG1), nullptr, DlgProc);
	g_hWnd = hWnd;

	if (!hWnd) {
		return FALSE;
	}
	return TRUE;
}
INT_PTR CALLBACK DlgProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam) {
	UNREFERENCED_PARAMETER(lParam);
	switch (message) {
	case WM_INITDIALOG:
	{
		g_hUnitStateAniList = GetDlgItem(hDlg, IDC_LIST1);
		// ===== List 설정 =====
		// 컬럼 추가(List 속성의 View를 Report로 설정해야 컬럼을 추가할 수 있다.)
		char szColumnText0[] = "EUnitStateType";
		char szColumnText1[] = "Ani File";
		LVCOLUMN column = {};
		column.mask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM;
		column.fmt = LVCFMT_LEFT;
		column.cx = 100;
		column.pszText = szColumnText0;
		ListView_InsertColumn(g_hUnitStateAniList, 0, &column); // 컬럼 추가0
		column.pszText = szColumnText1;
		ListView_InsertColumn(g_hUnitStateAniList, 1, &column); // 컬럼 추가1
		// 줄 전체가 클릭되도록 설정(기본값은 첫 번째 서브아이템의 텍스트 영역만 선택됨)
		ListView_SetExtendedListViewStyle(
			g_hUnitStateAniList,
			LVS_EX_FULLROWSELECT // 아이템 전체가 클릭되도록 한다.
			| LVS_EX_GRIDLINES // 서브아이템 사이에 그리드 라인을 넣는다.
		);
		// ===== List 설정 ===== end

		UpdateUnitStateAniList();

		return (INT_PTR)TRUE;
	}
	case WM_COMMAND:
	{
		int id = LOWORD(wParam);
		switch (id) {
		case IDC_EDIT1:
		{
			if (HIWORD(wParam) == EN_CHANGE) {
				char text[szMax_UnitName];
				GetDlgItemText(hDlg, IDC_EDIT1, text, szMax_UnitName);
				g_cUnitCreator.cUnit.SetName(text);
			}
		}
		break;
		case IDC_EDIT2:
		{
			//
		}
		break;
		case IDC_EDIT3:
		{
			if (HIWORD(wParam) == EN_CHANGE) {
				char text[szMax_Magnification];
				GetDlgItemText(hDlg, IDC_EDIT3, text, szMax_Magnification);
				g_cUnitCreator.cUnit.fMagnification = atof(text);
			}
		}
		break;
		case IDC_EDIT4:
		{
			if (HIWORD(wParam) == EN_CHANGE) {
				char text[FLT_MAX_10_EXP];
				GetDlgItemText(hDlg, IDC_EDIT4, text, FLT_MAX_10_EXP);
				g_cUnitCreator.cUnit.fSpeedPerSeconds = atof(text);
			}
		}
		break;
		case IDC_BUTTON1: // Load
		{
			char filePath[MAX_PATH] = {};
			if (OpenFileDialog(filePath)) {
				LoadUnit(filePath);
			};
		}
		break;
		case IDC_BUTTON2: // Save
		{
			char filePath[MAX_PATH] = {};
			if (OpenFileDialog(filePath)) {
				SaveUnit(filePath);
			}
		}
		break;
		case IDC_BUTTON3: // Load .ani
		{
			UINT itemIndex = ListView_GetNextItem(
				g_hUnitStateAniList, // 윈도우 핸들
				-1, // 검색을 시작할 인덱스
				LVNI_SELECTED // 검색 조건
			);
			if (itemIndex != NoSelectedIndex) {
				char filePath[MAX_PATH] = {};
				char fileTitle[MAX_PATH] = {};
				if (OpenFileDialog(filePath, fileTitle)) {
					g_cUnitCreator.cUnit.LoadAniFile((EUnitStateType)itemIndex, filePath);

					UpdateUnitStateAniList();
				};
			}

		}
		break;
		case IDC_BUTTON4: // Delete .ani
		{
			UINT itemIndex = ListView_GetNextItem(
				g_hUnitStateAniList, // 윈도우 핸들
				-1, // 검색을 시작할 인덱스
				LVNI_SELECTED // 검색 조건
			);

			if (itemIndex != NoSelectedIndex) {
				g_cUnitCreator.cUnit.arAniInfos[itemIndex].FilePath[0] = 0;
				g_cUnitCreator.cUnit.arAniInfos[itemIndex].FileTitle[0] = 0;

				UpdateUnitStateAniList();
			}
		}
		break;
		case IDC_BUTTON5: // Play Ani
		{
			//UINT itemIndex = ListView_GetNextItem(
			//	g_hUnitStateAniList, // 윈도우 핸들
			//	-1, // 검색을 시작할 인덱스
			//	LVNI_SELECTED // 검색 조건
			//);
			//g_cUnitCreator.cUnit.Clear((EUnitStateType)itemIndex);
			g_cUnitCreator.PlayStop(true);
			g_cUnitCreator.cUnit.Clear(EUnitStateType::Idle);
		}
		break;
		case IDC_BUTTON10: // Stop Ani
		{
			g_cUnitCreator.PlayStop(false);
			//g_cUnitCreator.cUnit.Stop();
		}
		break;
		case IDC_BUTTON9: // Load Bitmap
		{
			char filePath[MAX_PATH] = {};
			if (OpenFileDialog(filePath)) {
				g_cUnitCreator.cUnit.LoadUnitBitmap(filePath);

				UpdateBitmap();
			}
		}
		break;
		case IDC_BUTTON6: // Load .usp
		{
			char filePath[MAX_PATH] = {};
			if (OpenFileDialog(filePath)) {
				g_cUnitCreator.cUnit.cUnitStatePattern.LoadUnitStatePatternFile(filePath);
				UpdateUI();
			}
		}
		break;
		case IDC_BUTTON7: // Delete .usp
		{
			g_cUnitCreator.cUnit.cUnitStatePattern.Clear();
			UpdateUI();
		}
		break;
		default:
			break;
		}
	}
	break;
	case WM_CLOSE:
	{
		PostQuitMessage(0);
		return (INT_PTR)TRUE;
	}
	// 다이얼로그에서는 WM_DESTROY, default 처리를 하지 않는다.
	// 파일 다이얼로그창에서 이벤트가 발생하지 않아서 인터페이스 조작이 안된다.
	//case WM_DESTROY:
		//PostQuitMessage(0);
		//break;
	//default:
		//return DefWindowProc(hDlg, message, wParam, lParam);
	}
	return (INT_PTR)FALSE;
}
void LoadSettings(const char *filePath) {
	// 프로그램의 작업 경로
	char curDir[MAX_PATH] = {};
	GetCurrentDirectory(MAX_PATH, curDir);

	char fullPath[MAX_PATH] = {};
	sprintf_s(fullPath, "%s\\%s", curDir, filePath);

	FILE *file = nullptr;
	file = _fsopen(fullPath, "rt", _SH_DENYNO);
	if (file != nullptr) {
		fgets(g_szUnitFilePath, MAX_PATH, file);
		RemoveCarriageReturn(g_szUnitFilePath);

		fclose(file);
	}

}
void SaveSettings(const char *filePath) {
	char fullPath[MAX_PATH] = {};
	sprintf_s(fullPath, "%s\\%s", g_szCurDir, filePath);

	FILE *file = nullptr;
	file = _fsopen(fullPath, "wt", _SH_DENYNO);
	if (file != nullptr) {
		// image file name
		fputs(g_szUnitFilePath, file);
		fputs("\n", file);

		fclose(file);
	}
}
void LoadUnit(const char *filePath) {
	g_cUnitCreator.LoadUnit(filePath);
	//g_cUnitCreator.BindControllerAndUnit();

	UpdateUI();
	UpdateBitmap();
	UpdateUnitStateAniList();
}
void SaveUnit(const char *filePath) {
	g_cUnitCreator.SaveUnit(filePath);

	strcpy_s(g_szUnitFilePath, MAX_PATH, filePath);
	SaveSettings(g_szUnitFilePath);
}
void UpdateUI() {
	// name
	SetDlgItemText(g_hWnd, IDC_EDIT1, g_cUnitCreator.cUnit.szName);
	// speed
	char text[FLT_MAX_10_EXP];
	sprintf_s(text, FLT_MAX_10_EXP, "%f", g_cUnitCreator.cUnit.fSpeedPerSeconds);
	SetDlgItemText(g_hWnd, IDC_EDIT4, text);
	// magnification
	sprintf_s(text, FLT_MAX_10_EXP, "%f", g_cUnitCreator.cUnit.fMagnification);
	SetDlgItemText(g_hWnd, IDC_EDIT3, text);
	// file title
	SetDlgItemText(g_hWnd, IDC_EDIT2, g_cUnitCreator.cUnit.cUnitStatePattern.szFileTitle);
}
void UpdateBitmap() {
	HWND hWndPic2 = GetDlgItem(g_hWnd, IDC_PIC2);
	HDC hdc = GetDC(hWndPic2);
	TransparentBlt(hdc, 0, 0, 100, 100, g_cUnitCreator.cUnit.hBitmapDC, 0, 0, 100, 100, RGB(255, 0, 0));
}
void UpdateUnitStateAniList() {
	ListView_DeleteAllItems(g_hUnitStateAniList);

	LVITEM item = {};
	item.mask = LVIF_TEXT;
	item.state;
	item.stateMask;
	for (size_t i = 0; i < EUnitStateType::Count; i++) {
		item.iItem = i;
		item.iSubItem = 0; // 아이템을 처음 추가하므로 0번째 서브아이템을 선택한다.
		char itemText[MAX_PATH];
		strcpy_s(itemText, MAX_PATH, g_szUnitStateTypeAsString[i]);
		item.pszText = itemText;
		ListView_InsertItem(g_hUnitStateAniList, &item); // 아이템 추가0
		ListView_SetItemText(g_hUnitStateAniList, i, 1, g_cUnitCreator.cUnit.arAniInfos[i].FileTitle); // 아이템 추가0
	}
}