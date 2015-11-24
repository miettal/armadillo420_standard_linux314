#include <windows.h>
#include <windowsx.h>
#include <commctrl.h>
#include <stdio.h>
#include <host/common/erase.h>

#include "resource.h"
#include "sheet_erase.h"

#include "hermit.h"
#include "hermit_util.h"
#include "hermit_command.h"

HWND hDlg_Erase;
HWND hSheetErase;

static int EraseFunc(HWND hWnd);
DWORD WINAPI EraseThread(LPVOID arg);

LRESULT CALLBACK TabSheetProc_Erase(HWND hWnd, UINT msg, WPARAM wp, LPARAM lp){
	switch(msg){
	case WM_INITDIALOG:
		SendMessage(GetDlgItem(hWnd, IDC_ER_CB_REGION), CB_INSERTSTRING, (WPARAM)0, (LPARAM)"region ...");
		SendMessage(GetDlgItem(hWnd, IDC_ER_CB_REGION), CB_SETCURSEL, (WPARAM)0, (LPARAM)0);


		return TRUE;
	case WM_COMMAND:
		switch (GET_WM_COMMAND_ID(wp, lp)) {

		case IDC_ER_BT_ACTION:
			EraseFunc(hWnd);

			break;

		case IDC_ER_CB_REGION:
			switch(GET_WM_COMMAND_CMD(wp, lp)) {
			case CBN_DROPDOWN:
				char szCom[256];
				int ret;
				
				//Combobox Initialize
				SendMessage(GetDlgItem(hWnd, IDC_ER_CB_REGION), CB_RESETCONTENT, (WPARAM)0, (LPARAM)0);
				
				//get COM
				memset(szCom,0,256);
				GetDlgItemText(hDlg_Main,IDC_MA_CB_COM,szCom,256);
				
				//
				target_context_t *tc = NULL;
				ret = CommandMemmap(szCom,&tc);
				if(ret == FALSE) return FALSE;

				InitComboBox_MemRegion(hWnd,IDC_ER_CB_REGION,tc->memmap);
				CommandFinish(&tc);
				break;
			}
			return TRUE;
		}
		break;
	}
	return FALSE;
}

LRESULT CALLBACK DlgProc_Erase(HWND hWnd, UINT msg, WPARAM wp, LPARAM lp)
{
	switch (msg) {
	case WM_INITDIALOG:
		hDlg_Erase=hWnd;
		SendMessage(GetDlgItem(hWnd,IDC_PR_ERASE), PBM_SETRANGE,
			    (WPARAM)0,
			    MAKELPARAM(0, 100));
		return TRUE;
	case WM_CLOSE:
		EndDialog(hWnd, IDOK);
		return TRUE;
	}
	return FALSE;
}

static int EraseFunc(HWND hWnd)
{
	DWORD thread_id;
	char buf[256];

	if (GetDlgItemText(hWnd, IDC_ER_CB_REGION, buf, 256) == 0) {
		MessageBox(hWnd, "Please, input region.", "warning", MB_OK);
		return FALSE;
	}
	if (GetDlgItemText(hDlg_Main, IDC_MA_CB_COM, buf, 256) == 0) {
		MessageBox(hWnd, "Please, input COM port.", "warning", MB_OK);
		return FALSE;
	}

	CreateThread(NULL, 0, EraseThread, hWnd, 0, &thread_id);
	DialogBox((HINSTANCE)GetWindowLong(hWnd, GWL_HINSTANCE),
		  "DLG_ERASE", hWnd, (DLGPROC)DlgProc_Erase);

	return FALSE;
}

static void Callback_Erase(tsize_t total, tsize_t remain)
{
	int position;

	position = (total - remain) / (total / 100);

	SendMessage(GetDlgItem(hDlg_Erase, IDC_PR_ERASE),
		    PBM_SETPOS, (WPARAM)position, (LPARAM)0);
}

DWORD WINAPI EraseThread(LPVOID arg)
{
	HWND hWnd = (HWND)arg;
	target_context_t *tc = NULL;
	char szRegion[256];
	char szCom[256];
	char Log[1024];
	unsigned long ModeFlag=0;
	int use_region;
	int ret;

	memset(szRegion, 0, 256);
	memset(szCom, 0, 256);
	memset(Log, 0, 1024);

	GetDlgItemText(hWnd, IDC_ER_CB_REGION, szRegion, 256);
	GetDlgItemText(hDlg_Main, IDC_MA_CB_COM, szCom, 256);

	ModeFlag |= ((IsDlgButtonChecked(hWnd, IDC_ER_CH_LOCK) == BST_CHECKED)
		     ? F_FORCE_LOCKED : 0);

	/* Check RegionType */
	ret = CommandMemmap(szCom, &tc);
	if (ret == FALSE) {
		SendMessage(hDlg_Erase, WM_CLOSE, (WPARAM)0, (LPARAM)0);
		return FALSE;
	}

	use_region = Check_MemRegion(tc->memmap, szRegion);
	CommandFinish(&tc);

	/* Log output */
	sprintf(Log,
		"%s\t: %s\r\n"
		"Mode\t: %s\r\n""Com\t: %s\r\n",
		(use_region ? "Region" : "Address"), szRegion,
		((ModeFlag == BST_CHECKED) ? "ForceLock" : "default"), szCom);
	SetDlgItemText(hDlg_Erase, IDC_ED_ERASE_INFO, Log);

	register_erase_callback_function(Callback_Erase);

	sprintf(Log,
		"Erase START\r\n"
		"%s\t: %s\r\n"
		"Mode\t: %s\r\n""Com\t: %s",
		(use_region ? "Region" : "Address"), szRegion,
		((ModeFlag == BST_CHECKED) ? "ForceLock" : "default"), szCom);
	Display_Log(Log);

	if (use_region == TRUE) {
		/* region mode */
		ret = CommandErase_Region(szCom, &tc, szRegion, ModeFlag);
		CommandFinish(&tc);
	} else {
		/* address mode */
		char *endptr;
		unsigned long addr;
		int length = strlen(szRegion);

		addr = strtoul(szRegion, &endptr, 16);
		if ((endptr != (char *)((u_long)szRegion + (u_long)length)) ||
		    (addr == ULONG_MAX)) {
			MessageBox(hDlg_Erase, "unknown adress", "error", MB_OK);
			SendMessage(hDlg_Erase, WM_CLOSE, (WPARAM)0, (LPARAM)0);
			return FALSE;
		}

		ret = CommandErase_Address(szCom, &tc, addr, ModeFlag);
		CommandFinish(&tc);
	}

	if (ret == TRUE)
		Display_Log("Erase COMPLETE");
	else
		Display_Log("Erase FAILED");

	SendMessage(hDlg_Erase, WM_CLOSE, (WPARAM)0, (LPARAM)0);

	return TRUE;
}
