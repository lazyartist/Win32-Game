#include "stdafx.h"
#include "06-Tool-StageCreator.h"
#include "CStageCreator.h"
#include "Commctrl.h"

HINSTANCE g_hInst;
HWND g_hDlg;
HWND g_hCanvas;
CStageCreator g_cStageCreator;
char g_szCurDir[MAX_PATH] = {}; // 작업 경로, 프로그램 실행 중 파일 대화상자에서 선택한 곳으로 바뀌기 때문에 프로그램 실행과 동시에 저장해둔다.

INT_PTR CALLBACK DlgProc(HWND, UINT, WPARAM, LPARAM);
void UpdateUI();

int APIENTRY wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPWSTR lpCmdLine, _In_ int nCmdShow) {
	g_hInst = hInstance;
	g_hDlg = CreateDialog(g_hInst, MAKEINTRESOURCE(IDD_DIALOG1), nullptr, DlgProc);
	if (!g_hDlg) return 0;

	GetCurrentDirectory(MAX_PATH, g_szCurDir);

	g_hCanvas = GetDlgItem(g_hDlg, IDC_PIC1);
	g_cStageCreator.Init(g_hDlg, g_hCanvas, 1000 / 60, { 800, 600 }, EWindowMode::None);
	g_cStageCreator.PlayStop(true);
	if (g_cStageCreator.LoadSettings(g_szCurDir, Const::szStageSettingFileName())) {
		g_cStageCreator.LoadStage(g_cStageCreator.cStageFilePath);
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
			g_cStageCreator.SaveSettings(g_szCurDir, Const::szStageSettingFileName());
			return (INT_PTR)TRUE;
		}
		case IDC_BUTTON9: {//Load Background .bmp
			return (INT_PTR)TRUE;
		}
		case IDC_BUTTON4: {//Load .unit
			return (INT_PTR)TRUE;
		}
		case IDC_BUTTON8: {//Delete .unit
			return (INT_PTR)TRUE;
		}
		case IDC_BUTTON5: {//Set Playable Unit
			return (INT_PTR)TRUE;
		}
		case IDC_BUTTON6: {//Play
			return (INT_PTR)TRUE;
		}
		case IDC_BUTTON7: {//Stop
			return (INT_PTR)TRUE;
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

			if (pnmhdr->code == NM_CLICK) {
				// 클릭된 아이템 인덱스 알아내기
				UINT itemIndex = ListView_GetNextItem(
					pnmhdr->hwndFrom, // 윈도우 핸들
					-1, // 검색을 시작할 인덱스
					LVNI_SELECTED // 검색 조건
				);

				if (itemIndex != NoSelectedIndex) {
					//LoadSelectedUnit();
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
					//LoadSelectedUnit();
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
}