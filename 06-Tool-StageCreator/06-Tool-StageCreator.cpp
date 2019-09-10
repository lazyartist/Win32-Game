#include "stdafx.h"
#include "Windowsx.h"
#include "06-Tool-StageCreator.h"
#include "CStageCreator.h"
#include "lib.h"
#include "Commctrl.h"

HINSTANCE g_hInst;
HWND g_hDlg;
HWND g_hCanvas;
CStageCreator g_cStageCreator;
char g_szCurDir[MAX_PATH] = {}; // 작업 경로, 프로그램 실행 중 파일 대화상자에서 선택한 곳으로 바뀌기 때문에 프로그램 실행과 동시에 저장해둔다.
HWND g_hUnitList;
HWND g_hControlTypeCombo;

INT_PTR CALLBACK DlgProc(HWND, UINT, WPARAM, LPARAM);
void UpdateUI();
void UpdateUnitList();

int APIENTRY wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPWSTR lpCmdLine, _In_ int nCmdShow) {
	g_hInst = hInstance;
	g_hDlg = CreateDialog(g_hInst, MAKEINTRESOURCE(IDD_DIALOG1), nullptr, DlgProc);
	if (!g_hDlg) return 0;

	GetCurrentDirectory(MAX_PATH, g_szCurDir);

	g_hCanvas = GetDlgItem(g_hDlg, IDC_PIC1);
	g_cStageCreator.Init(g_hDlg, g_hCanvas, 1000 / 60, { 424 * 2, 318 * 2 }, EWindowMode::EWindowMode_None);
	g_cStageCreator.PlayStop(true);
	if (g_cStageCreator.LoadSettings(g_szCurDir, Const::szStageSettingFileName)) {
		g_cStageCreator.LoadStage(g_cStageCreator.cStageFilePath);
		UpdateUI();
		UpdateUnitList();
	}

	MSG msg;
	while (true) {
		if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)) {
			if (msg.message == WM_QUIT) break;
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		if (g_cStageCreator.UpdateFrame()) {
			g_cStageCreator.UpdateLogic();
			if (GetFocus() == g_hCanvas) {
				g_cStageCreator.UpdateController();
			}
			g_cStageCreator.UpdateRender();
		};
	}

	return (int)msg.wParam;
}
INT_PTR CALLBACK DlgProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam) {
	switch (message) {
	case WM_INITDIALOG: {
		g_hUnitList = GetDlgItem(hDlg, IDC_LIST1);
		// ===== List 설정 =====
		//g_hUnitList
		char szColumnName0[Const::szMax_ListColumnName] = "eControlType";
		char szColumnName1[Const::szMax_ListColumnName] = "eUnitType";
		char szColumnName2[Const::szMax_ListColumnName] = "FileTitle";
		char szColumnName3[Const::szMax_ListColumnName] = "startXY";
		LVCOLUMN column = {};
		column.mask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM;
		column.fmt = LVCFMT_LEFT;
		column.cx = 120;
		column.pszText = szColumnName0;
		ListView_InsertColumn(g_hUnitList, 0, &column);
		//column.cx = 150;
		column.pszText = szColumnName1;
		ListView_InsertColumn(g_hUnitList, 1, &column);
		////column.cx = 150;
		column.pszText = szColumnName2;
		ListView_InsertColumn(g_hUnitList, 2, &column);
		//column.cx = 150;
		column.pszText = szColumnName3;
		ListView_InsertColumn(g_hUnitList, 3, &column);
		// 줄 전체가 클릭되도록 설정(기본값은 첫 번째 서브아이템의 텍스트 영역만 선택됨)
		ListView_SetExtendedListViewStyle(
			g_hUnitList,
			LVS_EX_FULLROWSELECT // 아이템 전체가 클릭되도록 한다.
			| LVS_EX_GRIDLINES // 서브아이템 사이에 그리드 라인을 넣는다.
		);
		// ===== List 설정 ===== end
		// ===== ComboBox 설정 =====
		g_hControlTypeCombo = GetDlgItem(hDlg, IDC_COMBO1);
		for (size_t i = 0; i < EUnitType::EUnitType_Count; i++) {
			ComboBox_InsertString(g_hControlTypeCombo, i, Const::szControlTypesAsString[i]);
		}
		ComboBox_SetCurSel(g_hControlTypeCombo, 0);
		// ===== ComboBox 설정 ===== end
		return (INT_PTR)TRUE;
	}
	case WM_COMMAND: {
		switch (LOWORD(wParam)) {
		case IDC_BUTTON1: {//Load .stage
			CFilePath cFilePath;
			if (Func::OpenFileDialog(&cFilePath, "Load State\0*.stage\0")) {
				g_cStageCreator.LoadStage(cFilePath);
				UpdateUI();
			}
			return (INT_PTR)TRUE;
		}
		case IDC_BUTTON2: {//Save .stage
			CFilePath cFilePath;
			if (Func::OpenFileDialog(&cFilePath, "Save State\0*.stage\0")) {
				g_cStageCreator.SaveStage(cFilePath);
				UpdateUI();
			}
			return (INT_PTR)TRUE;
		}
		case IDC_BUTTON3: {//Save settings
			g_cStageCreator.SaveSettings(g_szCurDir, Const::szStageSettingFileName);
			return (INT_PTR)TRUE;
		}
		case IDC_BUTTON9: {//Load Background .bmp
			CFilePath cFilePath;
			if (Func::OpenFileDialog(&cFilePath, "Load background image\0*.bmp\0")) {
				g_cStageCreator.LoadBgi(cFilePath);
				UpdateUI();
			}
			return (INT_PTR)TRUE;
		}
		case IDC_BUTTON4: {//Add .unit
			CFilePath cFilePath;
			if (Func::OpenFileDialog(&cFilePath, "Add Unit\0*.unit\0")) {
				g_cStageCreator.AddUnit(cFilePath);
				UpdateUnitList();
			}
			return (INT_PTR)TRUE;
		}
		case IDC_BUTTON8: {//Remove .unit
			UINT itemIndex = ListView_GetNextItem(
				g_hUnitList, // 윈도우 핸들
				-1, // 검색을 시작할 인덱스
				LVNI_SELECTED // 검색 조건
			);
			if (itemIndex != NoSelectedIndex) {
				g_cStageCreator.RemoveUnit(itemIndex);
				UpdateUnitList();
			}
			return (INT_PTR)TRUE;
		}
		case IDC_BUTTON5: {//Set current position to StartXY
			UINT itemIndex = ListView_GetNextItem(g_hUnitList/*윈도우핸들*/, -1/*검색시작인덱스*/, LVNI_SELECTED/*검색조건*/);
			if (itemIndex != NoSelectedIndex) {
				g_cStageCreator.SetStartXY(itemIndex);
				UpdateUI();
				UpdateUnitList();
			}
			return (INT_PTR)TRUE;
		}
		case IDC_BUTTON11: {//Set ControlType
			UINT unitIndex = ListView_GetNextItem(g_hUnitList/*윈도우핸들*/, -1/*검색시작인덱스*/, LVNI_SELECTED/*검색조건*/);
			if (unitIndex != NoSelectedIndex) {
				UINT controlTypeIndex = ComboBox_GetCurSel(g_hControlTypeCombo);
				if (controlTypeIndex != NoSelectedIndex) {
					g_cStageCreator.SetControlType(unitIndex, (EControlType)controlTypeIndex);
					UpdateUI();
					UpdateUnitList();
				}
			}
			return (INT_PTR)TRUE;
		}
		case IDC_BUTTON6: {//Play
			g_cStageCreator.PlayStop(true);
			return (INT_PTR)TRUE;
		}
		case IDC_BUTTON7: {//Stop
			g_cStageCreator.PlayStop(false);
			return (INT_PTR)TRUE;
		}
		case IDC_BUTTON10: {//Reset
			g_cStageCreator.Reset();
			return (INT_PTR)TRUE;
		}
		case IDC_EDIT3:
		{
			if (HIWORD(wParam) == EN_CHANGE) {
				char text[Const::szMax_ItemLine];
				GetDlgItemText(hDlg, IDC_EDIT3, text, Const::szMax_ItemLine);
				g_cStageCreator.fBgiMagnification = atof(text);
			}
		}
		default:
			break;
		}
	}
	case WM_NOTIFY:
	{
		// ListView의 통지 메시지 받기
		if (wParam == IDC_LIST1) {
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

			if (pnmhdr->code == NM_CLICK || pnmhdr->code == LVN_ITEMCHANGED) {
				// 선택된 아이템 인덱스 알아내기
				UINT itemIndex = ListView_GetNextItem(pnmhdr->hwndFrom/*윈도우핸들*/, -1/*검색시작인덱스*/, LVNI_SELECTED/*검색조건*/);
				if (itemIndex != NoSelectedIndex) {
					//g_cStageCreator.SetControlType(unitIndex);
					//UpdateUI();
					//UpdateUnitList();
				}
			}
		}
	}
	break;
	// 공통 컨트롤의 통지 메시지는 WM_NOTIFY로 받는다.
	case WM_LBUTTONDOWN:
	{
		SetFocus(g_hCanvas); // 리스트에서 포커스를 제거하기 위해 윈도우에 포커스를 준다.
		break;
	}
	case WM_CLOSE: {
		PostQuitMessage(0);
		return (INT_PTR)TRUE;
		break;
	}
	}

	return (INT_PTR)FALSE;
}
void UpdateUI() {
	//.stage name
	SetDlgItemText(g_hDlg, IDC_EDIT1, g_cStageCreator.cStageFilePath.szFileTitle);
	//bgi
	SetDlgItemText(g_hDlg, IDC_EDIT2, g_cStageCreator.cBgiFilePath.szFileTitle);
	char text[Const::szMax_ItemLine];
	sprintf_s(text, Const::szMax_ItemLine, "%.2f", g_cStageCreator.fBgiMagnification);
	SetDlgItemText(g_hDlg, IDC_EDIT3, text);
}
void UpdateUnitList() {
	ListView_DeleteAllItems(g_hUnitList);
	LVITEM item = {};
	item.mask = LVIF_TEXT;
	item.state;
	item.stateMask;
	int iCount = g_cStageCreator.cUnits.size();
	for (size_t i = 0; i < iCount; i++) {
		CUnit &cUnit = g_cStageCreator.cUnits[i];
		char itemText[Const::szMax_ItemLine] = {};
		item.iItem = i;
		item.iSubItem = 0; // 아이템을 처음 추가하므로 0번째 서브아이템을 선택한다.
		item.pszText = itemText;
		ListView_InsertItem(g_hUnitList, &item);
		strcpy_s(itemText, MAX_PATH, Const::szControlTypesAsString[cUnit.eControlType]);
		ListView_SetItemText(g_hUnitList, i, 0, itemText);
		strcpy_s(itemText, MAX_PATH, Const::szUnitTypesAsString[cUnit.eUnitType]);
		ListView_SetItemText(g_hUnitList, i, 1, itemText);
		ListView_SetItemText(g_hUnitList, i, 2, cUnit.cFilePath.szFileTitle);
		sprintf_s(itemText, Const::szMax_ItemLine, "%.1f, %.1f", cUnit.sStartXY.x, cUnit.sStartXY.y);
		ListView_SetItemText(g_hUnitList, i, 3, itemText);
	}
}