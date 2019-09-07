#include "stdafx.h"
#include "05-Tool-UnitCreator.h"
#include "CUnitCreator.h"
#include "lib.h"
#include "Commctrl.h"

HINSTANCE g_hInstance;
const char *g_szActionTypeAsString[] = { "EActionType_Idle" , "EActionType_MoveTo", "EActionType_Shoot" };
HWND g_hDlg;
HWND g_hWndPicture;
HWND g_hUnitList;
HWND g_hActionAniList;
CUnitCreator g_cUnitCreator;
char g_szCurDir[MAX_PATH] = {}; // 작업 경로, 프로그램 실행 중 파일 대화상자에서 선택한 곳으로 바뀌기 때문에 프로그램 실행과 동시에 저장해둔다.
char g_szUnitFilePath[MAX_PATH];

BOOL InitInstance(HINSTANCE, int);
INT_PTR CALLBACK DlgProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
void LoadSettings();
void SaveSettings();
void AddUnit(const char *filePath, const char *fileTitle);
void DeleteUnit(int index);
void SaveUnit(const char *filePath);
void LoadSelectedUnit();
void UpdateUnitList();
void UpdateUnitUI();
void UpdateBitmap();
void UpdateActionAniList();

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPWSTR    lpCmdLine,
	_In_ int       nCmdShow) {
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

	if (!InitInstance(hInstance, nCmdShow)) {
		return FALSE;
	}

	g_hWndPicture = GetDlgItem(g_hDlg, IDC_PIC1);
	g_cUnitCreator.Init(g_hDlg, g_hWndPicture, 1000 / 90, { 800, 600 }, EWindowMode::None);
	LoadSettings();
	UpdateUnitList();

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
		if (g_cUnitCreator.UpdateFrame()) {
			g_cUnitCreator.UpdateLogic();
			if (GetFocus() == g_hWndPicture) {
				g_cUnitCreator.UpdateController();
			}
			g_cUnitCreator.UpdateRender();
		};
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
	g_hDlg = hWnd;

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
		g_hUnitList = GetDlgItem(hDlg, IDC_LIST2);
		g_hActionAniList = GetDlgItem(hDlg, IDC_LIST1);
		// ===== List 설정 =====
		//g_hUnitList
		char szColumnName0[Const::szMax_ListColumnName] = {};
		char szColumnName1[Const::szMax_ListColumnName] = {};
		// 컬럼 추가(List 속성의 View를 Report로 설정해야 컬럼을 추가할 수 있다.)
		strcpy_s(szColumnName0, Const::szMax_ListColumnName, ".unit FileTitle");
		strcpy_s(szColumnName1, Const::szMax_ListColumnName, "FilePath");
		LVCOLUMN column = {};
		column.mask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM;
		column.fmt = LVCFMT_LEFT;
		column.cx = 150;
		column.pszText = szColumnName0;
		ListView_InsertColumn(g_hUnitList, 0, &column); // 컬럼 추가0
		column.cx = 300;
		column.pszText = szColumnName1;
		ListView_InsertColumn(g_hUnitList, 1, &column); // 컬럼 추가1
		// 줄 전체가 클릭되도록 설정(기본값은 첫 번째 서브아이템의 텍스트 영역만 선택됨)
		ListView_SetExtendedListViewStyle(
			g_hUnitList,
			LVS_EX_FULLROWSELECT // 아이템 전체가 클릭되도록 한다.
			| LVS_EX_GRIDLINES // 서브아이템 사이에 그리드 라인을 넣는다.
		);

		//g_hActionAniList
		// 컬럼 추가(List 속성의 View를 Report로 설정해야 컬럼을 추가할 수 있다.)
		strcpy_s(szColumnName0, Const::szMax_ListColumnName, "EActionType");
		strcpy_s(szColumnName1, Const::szMax_ListColumnName, "Ani File");
		column.cx = 200;
		column.pszText = szColumnName0;
		ListView_InsertColumn(g_hActionAniList, 0, &column); // 컬럼 추가0
		column.cx = 150;
		column.pszText = szColumnName1;
		ListView_InsertColumn(g_hActionAniList, 1, &column); // 컬럼 추가1
		// 줄 전체가 클릭되도록 설정(기본값은 첫 번째 서브아이템의 텍스트 영역만 선택됨)
		ListView_SetExtendedListViewStyle(
			g_hActionAniList,
			LVS_EX_FULLROWSELECT // 아이템 전체가 클릭되도록 한다.
			| LVS_EX_GRIDLINES // 서브아이템 사이에 그리드 라인을 넣는다.
		);
		// ===== List 설정 ===== end

		UpdateActionAniList();

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
		case IDC_BUTTON1: // Add Unit
		{
			char filePath[MAX_PATH] = {};
			char fileTitle[MAX_PATH] = {};
			if (OpenFileDialog(filePath, fileTitle)) {
				AddUnit(filePath, fileTitle);
				UpdateUnitList();
				UpdateUnitUI();
				UpdateBitmap();
				UpdateActionAniList();
			};
		}
		break;
		case IDC_BUTTON11: // Delete Unit
		{
			UINT itemIndex = ListView_GetNextItem(
				g_hUnitList, // 윈도우 핸들
				-1, // 검색을 시작할 인덱스
				LVNI_SELECTED // 검색 조건
			);
			if (itemIndex != NoSelectedIndex) {
				DeleteUnit(itemIndex);
				UpdateUnitList();
				UpdateUnitUI();
				UpdateBitmap();
				UpdateActionAniList();
			}
		}
		break;
		case IDC_BUTTON2: // Save Unit
		{
			char filePath[MAX_PATH] = {};
			if (OpenFileDialog(filePath)) {
				SaveUnit(filePath);
			}
		}
		break;
		case IDC_BUTTON17: // New Unit
		{
			g_cUnitCreator.cUnit.Clear();
			UpdateUnitList();
			UpdateUnitUI();
			UpdateBitmap();
			UpdateActionAniList();
			UpdateActionAniList();
		}
		break;
		case IDC_BUTTON3: // Load .ani
		{
			UINT itemIndex = ListView_GetNextItem(
				g_hActionAniList, // 윈도우 핸들
				-1, // 검색을 시작할 인덱스
				LVNI_SELECTED // 검색 조건
			);
			if (itemIndex != NoSelectedIndex) {
				char filePath[MAX_PATH] = {};
				char fileTitle[MAX_PATH] = {};
				if (OpenFileDialog(filePath, fileTitle)) {
					g_cUnitCreator.cUnit.LoadAniFile((EActionType)itemIndex, filePath);

					UpdateActionAniList();
				};
			}
		}
		break;
		case IDC_BUTTON4: // Delete .ani
		{
			UINT itemIndex = ListView_GetNextItem(
				g_hActionAniList, // 윈도우 핸들
				-1, // 검색을 시작할 인덱스
				LVNI_SELECTED // 검색 조건
			);

			if (itemIndex != NoSelectedIndex) {
				g_cUnitCreator.cUnit.arAniInfos[itemIndex].FilePath[0] = 0;
				g_cUnitCreator.cUnit.arAniInfos[itemIndex].FileTitle[0] = 0;

				UpdateActionAniList();
			}
		}
		break;
		case IDC_BUTTON5: // Play Ani
		{
			//UINT itemIndex = ListView_GetNextItem(
			//	g_hActionAniList, // 윈도우 핸들
			//	-1, // 검색을 시작할 인덱스
			//	LVNI_SELECTED // 검색 조건
			//);
			g_cUnitCreator.PlayStop(true);
			g_cUnitCreator.cUnit.Reset();
		}
		break;
		case IDC_BUTTON10: // Stop Ani
		{
			g_cUnitCreator.PlayStop(false);
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
				g_cUnitCreator.cUnit.cActionListPattern.LoadActionPatternFile(filePath);
				UpdateUnitUI();
			}
		}
		break;
		case IDC_BUTTON7: // Delete .usp
		{
			g_cUnitCreator.cUnit.cActionListPattern.Clear();
			UpdateUnitUI();
		}
		break;
		case IDC_BUTTON16: // Apply .usp
		{
			g_cUnitCreator.cUnit.cActionList = g_cUnitCreator.cUnit.cActionListPattern;
		}
		break;
		case IDC_BUTTON8: // Loas settings
		{
			LoadSettings();
		}
		break;
		case IDC_BUTTON13: // Save settings
		{
			SaveSettings();
		}
		break;
		default:
			break;
		}
	}
	break;
	// 공통 컨트롤의 통지 메시지는 WM_NOTIFY로 받는다.
	case WM_LBUTTONDOWN:
	{
		SetFocus(g_hWndPicture); // 리스트에서 포커스를 제거하기 위해 윈도우에 포커스를 준다.
	}
	break;
	case WM_NOTIFY:
	{
		// ListView의 통지 메시지 받기
		if (wParam == IDC_LIST2) {
			NMHDR *pnmhdr = nullptr;

			// NMHDR을 얻는 방법1.
			NMTTDISPINFO *nmttdispinfo = (NMTTDISPINFO*)lParam;
			pnmhdr = &(nmttdispinfo->hdr);

			// NMHDR을 얻는 방법2.
			// NMTTDISPINFO는 NMHDR을 확장하는 구조체로서
			// 첫 멤버변수로 NMHDR을 가지고 있기 때문에 NMHDR 형으로도 캐스팅이 가능하다.
			pnmhdr = (NMHDR*)lParam;

			pnmhdr->hwndFrom; // 윈도우 핸들
			pnmhdr->idFrom; // 컨트롤 아이디
			pnmhdr->code; // 통지 코드

			if (pnmhdr->code == NM_CLICK) {
				// 클릭된 아이템 인덱스 알아내기
				UINT itemIndex = ListView_GetNextItem(
					pnmhdr->hwndFrom, // 윈도우 핸들
					-1, // 검색을 시작할 인덱스
					LVNI_SELECTED // 검색 조건
				);

				if (itemIndex != NoSelectedIndex) {
					LoadSelectedUnit();
					//SetFocus(g_hDlg); // 리스트에서 포커스를 제거하기 위해 윈도우에 포커스를 준다.
					//ListView_SetItemState(pnmhdr->hwndFrom,         // handle to listview
						//-1,         // index to listview item
						//0, // item state
						//LVIS_SELECTED | LVIS_FOCUSED);                      // mask
				}
			}
			else if (pnmhdr->code == LVN_ITEMCHANGED) {
				UINT itemIndex = ListView_GetNextItem(
					pnmhdr->hwndFrom, // 윈도우 핸들
					-1, // 검색을 시작할 인덱스
					LVNI_SELECTED // 검색 조건
				);
				if (itemIndex != NoSelectedIndex) {
					LoadSelectedUnit();
				}
			}
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
void LoadSettings() {
	g_cUnitCreator.LoadSettings("settings.cfg");
}
void SaveSettings() {
	g_cUnitCreator.SaveSettings(g_szCurDir, "settings.cfg");
}
void LoadSelectedUnit() {
	UINT itemIndex = ListView_GetNextItem(
		g_hUnitList, // 윈도우 핸들
		-1, // 검색을 시작할 인덱스
		LVNI_SELECTED // 검색 조건
	);
	if (itemIndex != NoSelectedIndex) {
		CFilePath cFilePath = g_cUnitCreator.vecUnitFilePaths[itemIndex];
		g_cUnitCreator.LoadUnit(cFilePath.szFilePath);
		UpdateUnitUI();
		UpdateBitmap();
		UpdateActionAniList();
	}
}
void AddUnit(const char *filePath, const char *fileTitle) {
	CFilePath cFilePath;
	strcpy_s(cFilePath.szFilePath, Const::szMax_Path, filePath);
	strcpy_s(cFilePath.szFileTitle, Const::szMax_Path, fileTitle);
	g_cUnitCreator.AddUnitFilePath(cFilePath);
}
void DeleteUnit(int index) {
	g_cUnitCreator.DeleteUnitFilePath(index);
}
void SaveUnit(const char *filePath) {
	g_cUnitCreator.SaveUnit(filePath);
}
void UpdateUnitList() {
	ListView_DeleteAllItems(g_hUnitList);
	for (size_t i = 0; i < g_cUnitCreator.vecUnitFilePaths.size(); i++) {
		LVITEM item = {};
		item.mask = LVIF_TEXT;
		item.state;
		item.stateMask;
		item.iItem = i;
		item.iSubItem = 0; // 아이템을 처음 추가하므로 0번째 서브아이템을 선택한다.
		item.pszText = g_cUnitCreator.vecUnitFilePaths[i].szFileTitle;
		ListView_InsertItem(g_hUnitList, &item); // 아이템 추가0
		//item.pszText = g_cUnitCreator.vecUnitFilePaths[i].szFilePath;
		ListView_SetItemText(g_hUnitList, i, 1, g_cUnitCreator.vecUnitFilePaths[i].szFilePath); // 아이템 추가1
	}
}
void UpdateUnitUI() {
	// name
	SetDlgItemText(g_hDlg, IDC_EDIT1, g_cUnitCreator.cUnit.szName);
	// speed
	char text[FLT_MAX_10_EXP];
	sprintf_s(text, FLT_MAX_10_EXP, "%f", g_cUnitCreator.cUnit.fSpeedPerSeconds);
	SetDlgItemText(g_hDlg, IDC_EDIT4, text);
	// magnification
	sprintf_s(text, FLT_MAX_10_EXP, "%f", g_cUnitCreator.cUnit.fMagnification);
	SetDlgItemText(g_hDlg, IDC_EDIT3, text);
	// file title
	SetDlgItemText(g_hDlg, IDC_EDIT2, g_cUnitCreator.cUnit.cActionListPattern.szFileTitle);
}
void UpdateBitmap() {
	HWND hWndPic2 = GetDlgItem(g_hDlg, IDC_PIC2);
	HDC hdc = GetDC(hWndPic2);
	TransparentBlt(hdc, 0, 0, 100, 100, g_cUnitCreator.cUnit.hBitmapDC, 0, 0, 100, 100, RGB(255, 0, 0));
}
void UpdateActionAniList() {
	ListView_DeleteAllItems(g_hActionAniList);

	LVITEM item = {};
	item.mask = LVIF_TEXT;
	item.state;
	item.stateMask;
	for (size_t i = 0; i < EActionType::Count; i++) {
		item.iItem = i;
		item.iSubItem = 0; // 아이템을 처음 추가하므로 0번째 서브아이템을 선택한다.
		char itemText[MAX_PATH];
		strcpy_s(itemText, MAX_PATH, g_szActionTypeAsString[i]);
		item.pszText = itemText;
		ListView_InsertItem(g_hActionAniList, &item); // 아이템 추가0
		ListView_SetItemText(g_hActionAniList, i, 1, g_cUnitCreator.cUnit.arAniInfos[i].FileTitle); // 아이템 추가0
	}
}