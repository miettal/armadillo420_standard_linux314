#include <windows.h>
#include <windowsx.h>

#include <stdio.h>
#include "resource.h"
#include "sheet_version.h"

HWND hSheetVersion;

LRESULT CALLBACK TabSheetProc_Version(HWND hWnd, UINT msg, WPARAM wp, LPARAM lp){
	switch(msg){
	case WM_INITDIALOG:
		char szBuf[256];
		memset(szBuf,0,256);
		sprintf(szBuf,"version 0.00\r\n");
		Edit_SetText(GetDlgItem(hWnd,IDC_ED_VER),szBuf);
		return TRUE;
#if 0
	case WM_COMMAND:
		switch (GET_WM_COMMAND_ID(wp, lp)) {
		case IDOK:
			EndDialog(hWnd, IDOK);
			return TRUE;
		}
		break;
#endif
	}
	return FALSE;
}