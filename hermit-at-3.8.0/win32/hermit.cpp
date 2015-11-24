#include <windows.h>
#include <windowsx.h>
#include <commctrl.h>

#include <stdio.h>

#include "resource.h"

#include "sheet_download.h"
#include "sheet_erase.h"
#include "sheet_shoehorn.h"

#include "hermit.h"
#include "hermit_command.h"
#include "hermit_util.h"


LRESULT CALLBACK DlgProc_Main(HWND, UINT, WPARAM, LPARAM);

HWND hDlg_Main;

char AppDir[APP_DIR_LEN];

#define SHEET_NUM 5

struct TabControlSheetInfo tcs_info[SHEET_NUM] = {
	{ hSheetDownload, TCS_DOWNLOAD, SheetName_Download,SheetID_Download,(TabSheetProcFunc)TabSheetProc_Download},
	{ hSheetErase, TCS_ERASE, SheetName_Erase, SheetID_Erase, (TabSheetProcFunc)TabSheetProc_Erase},
	{ NULL, TCS_NONE, SheetName_Memmap, SheetID_Memmap, NULL},
	{ hSheetShoehorn, TCS_SHOEHORN, SheetName_Shoehorn,SheetID_Shoehorn,(TabSheetProcFunc)TabSheetProc_Shoehorn},
	{ NULL, TCS_NONE, SheetName_Version,SheetID_Version,NULL},
	
};

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
//+ 
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
int WINAPI WinMain(HINSTANCE hCurInst, HINSTANCE hPrevInst,
                   LPSTR lpsCmdLine, int nCmdShow)
{
	GetCurrentDirectory(APP_DIR_LEN, AppDir);
	
    DialogBox(hCurInst, "DLG_MAIN", NULL, (DLGPROC)DlgProc_Main);
    return 0;
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
//+ 
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
LRESULT CALLBACK DlgProc_Main(HWND hWnd, UINT msg, WPARAM wp, LPARAM lp)
{
	HICON hIcon;
	
	static HWND TabCtrl;
	
	HINSTANCE hInst = (HINSTANCE)GetWindowLong(hWnd,GWL_HINSTANCE);
	
    switch (msg) {
	case WM_INITDIALOG:
		hDlg_Main=hWnd;
		
		//Set Icon
		hIcon = LoadIcon(hInst, MAKEINTRESOURCE(IDI_HERMIT));
		SendMessage(hWnd, WM_SETICON, ICON_SMALL, (LPARAM)hIcon);
		
		InitComboBox_SerialComm(hWnd, IDC_MA_CB_COM);
		
		InitCommonControls();
		TCITEM tc;
		
		TabCtrl = GetDlgItem(hWnd,IDC_MA_TC_EVENT);
		int i;
		for(i=0; i<SHEET_NUM; i++){
			tc.mask = TCIF_TEXT;
			tc.pszText=tcs_info[i].title;
			TabCtrl_InsertItem(TabCtrl,tcs_info[i].id,&tc);
			
			if(tcs_info[i].sheet == TCS_NONE) continue;
			
			tcs_info[i].hWnd = CreateDialog(hInst,(LPCTSTR)tcs_info[i].sheet,
				hWnd,(DLGPROC)tcs_info[i].proc);
			
			RECT rt;
			GetClientRect(TabCtrl,&rt);
			TabCtrl_AdjustRect(TabCtrl,FALSE,&rt);
			MapWindowPoints(TabCtrl,hWnd,(LPPOINT)&rt,2);
			
			MoveWindow(tcs_info[i].hWnd,rt.left,rt.top,rt.right-rt.left,rt.bottom-rt.top,FALSE);
			
		}
		
		ShowWindow(tcs_info[0].hWnd,SW_SHOW);
		
		return TRUE;
		
	case WM_NOTIFY:
		switch(((NMHDR *)lp)->code){
		case TCN_SELCHANGE:
			if(((NMHDR *)lp)->hwndFrom == TabCtrl){
				int i;
				static int Previous;
				int Current;
				
				Current = TabCtrl_GetCurSel(TabCtrl);
				switch(Current){
				case SheetID_Version:
					ShowWindow(tcs_info[Previous].hWnd,SW_SHOW);
					TabCtrl_SetCurSel(TabCtrl,Previous);
					
					Display_Version();
					return TRUE;
				case SheetID_Memmap:
					int ret;
					char szCom[256];
					GetDlgItemText(hDlg_Main,IDC_MA_CB_COM,szCom,256);
					ShowWindow(tcs_info[Previous].hWnd,SW_SHOW);
					TabCtrl_SetCurSel(TabCtrl,Previous);
					
					target_context_t *tc;
					tc = NULL;
					ret = CommandMemmap(szCom,&tc);
					if(ret == FALSE) return FALSE;
					
					Display_MemRegion(tc->memmap);
					CommandFinish(&tc);
					return TRUE;
				default:
					break;
				}
				
				for(i=0; i<SHEET_NUM; i++){
					ShowWindow(tcs_info[i].hWnd,SW_HIDE);
				}
				
				if(0 <= Current && Current < SHEET_NUM){
					Previous = Current;
					ShowWindow(tcs_info[Current].hWnd,SW_SHOW);
				}
				
			}
			break;
		}
		return TRUE;
		
		case WM_CLOSE:
			EndDialog(hWnd, IDOK);
			break;
			
		case WM_COMMAND:
			return TRUE;
	}
	return FALSE;
}
