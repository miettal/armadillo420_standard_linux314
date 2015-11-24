#include <windows.h>
#include <windowsx.h>
#include <commctrl.h>

#include <stdio.h>

#include <host/common/download.h>

#include "resource.h"
#include "sheet_download.h"

#include "hermit.h"
#include "hermit_util.h"
#include "hermit_command.h"

HWND hDlg_Download;
HWND hSheetDownload;

static int DownloadFunc(HWND hWnd);
DWORD WINAPI DownloadThread(LPVOID arg);


LRESULT CALLBACK TabSheetProc_Download(HWND hWnd, UINT msg, WPARAM wp, LPARAM lp){
	switch(msg){
	case WM_INITDIALOG:
		Edit_SetText(GetDlgItem(hWnd,IDC_DL_EB_PATH),"download file ...");

		//
		SendMessage(GetDlgItem(hWnd, IDC_DL_CB_REGION), CB_INSERTSTRING, (WPARAM)0, (LPARAM)"region ...");
		SendMessage(GetDlgItem(hWnd, IDC_DL_CB_REGION), CB_SETCURSEL, (WPARAM)0, (LPARAM)0);


		return TRUE;


	case WM_COMMAND:
		switch (GET_WM_COMMAND_ID(wp, lp)) {
		case IDC_DL_EB_PATH:
			switch(GET_WM_COMMAND_CMD(wp, lp)){
			case EN_SETFOCUS:
				int line_len;
				line_len = SendMessage(GetDlgItem(hWnd,IDC_DL_EB_PATH), EM_LINELENGTH, (WPARAM)0, (LPARAM)0);
				SendMessage(GetDlgItem(hWnd,IDC_DL_EB_PATH), EM_SETSEL, (WPARAM)0, (LPARAM)line_len);
				return TRUE;
			}
			return TRUE;

		case IDC_DL_BT_FILE:
			GetFileInfo(hWnd,GetDlgItem(hWnd,IDC_DL_EB_PATH));
			return TRUE;

		case IDC_DL_BT_ACTION:
			DownloadFunc(hWnd);
			break;

		case IDC_DL_CB_REGION:
			switch(GET_WM_COMMAND_CMD(wp, lp)) {
			case CBN_DROPDOWN:
				char szCom[256];
				int ret;
				
				//Combobox Initialize
				SendMessage(GetDlgItem(hWnd, IDC_DL_CB_REGION), CB_RESETCONTENT, (WPARAM)0, (LPARAM)0);
				
				//get COM
				memset(szCom,0,256);
				GetDlgItemText(hDlg_Main,IDC_MA_CB_COM,szCom,256);
				
				//
				target_context_t *tc = NULL;
				ret = CommandMemmap(szCom,&tc);
				if(ret == FALSE) return FALSE;

				InitComboBox_MemRegion(hWnd,IDC_DL_CB_REGION,tc->memmap);
				//Display_MemRegion(tc->memmap);
				CommandFinish(&tc);
				break;
			}
			return TRUE;


		}
		break;

	}
	return FALSE;
}
LRESULT CALLBACK DlgProc_Download(HWND hWnd, UINT msg, WPARAM wp, LPARAM lp)
{
    switch (msg) {
	case WM_INITDIALOG:
		hDlg_Download=hWnd;
		SendMessage(GetDlgItem(hWnd,IDC_PR_DOWNLOAD), PBM_SETRANGE,
			(WPARAM)0,
			MAKELPARAM(0, 100));
		return TRUE;
	case WM_CLOSE:
		EndDialog(hWnd, IDOK);
		return TRUE;
	}
	return FALSE;
}

static int DownloadFunc(HWND hWnd){
	char buf[256];
	if(	GetDlgItemText(hWnd,IDC_DL_EB_PATH,buf,256)==0){
		MessageBox(hWnd,"Please, input Image file.","warning",MB_OK);
		return FALSE;
	}
	if(GetDlgItemText(hWnd,IDC_DL_CB_REGION,buf,256)==0){
		MessageBox(hWnd,"Please, input region.","warning",MB_OK);
		return FALSE;
	}
	if(GetDlgItemText(hDlg_Main,IDC_MA_CB_COM,buf,256)==0){
		MessageBox(hWnd,"Please, input COM port.","warning",MB_OK);
		return FALSE;
	}
	
	DWORD thread_id;
	CreateThread(NULL,0,DownloadThread,hWnd,0,&thread_id);
	DialogBox((HINSTANCE)GetWindowLong(hWnd,GWL_HINSTANCE), "DLG_DOWNLOAD",hWnd, (DLGPROC)DlgProc_Download);
	
	return FALSE;
}

void Callback_Download(void *arg1, void *arg2){
	int total   = *(int *)arg1;
	int remain  = *(int *)arg2;
	int position;

	position = (total - remain) / (total / 100);

	SendMessage(GetDlgItem(hDlg_Download,IDC_PR_DOWNLOAD), PBM_SETPOS, (WPARAM)position, (LPARAM)0);
}

DWORD WINAPI DownloadThread(LPVOID arg){
	HWND hWnd = (HWND)arg;
	char szFile[256];
	char szRegion[256];
	char szCom[256];
	char Log[1024];
	unsigned long ModeFlag=0;
	unsigned long Size;
	
	memset(szFile,0,256);
	memset(szRegion,0,256);
	memset(szCom,0,256);
	memset(szFile,0,256);
	memset(Log,0,1024);
	
	GetDlgItemText(hWnd,IDC_DL_EB_PATH,szFile,256);
	GetDlgItemText(hWnd,IDC_DL_CB_REGION,szRegion,256);
	GetDlgItemText(hDlg_Main,IDC_MA_CB_COM,szCom,256);

	ModeFlag |= ((IsDlgButtonChecked(hWnd,IDC_DL_CH_LOCK)==BST_CHECKED)?F_FORCE_LOCKED:0);

	int ret;
	HANDLE hFile;
	hFile=CreateFile(szFile,GENERIC_READ,0,NULL,OPEN_EXISTING,FILE_ATTRIBUTE_READONLY,0);
	if(hFile == INVALID_HANDLE_VALUE){
		MessageBox(hWnd,"file not open","error",MB_OK);
		SendMessage(hDlg_Download, WM_CLOSE, (WPARAM)0, (LPARAM)0);
		return FALSE;
	}
	Size = GetFileSize(hFile, NULL); 

	CloseHandle(hFile);	

	//Log output
	sprintf(Log,
		"File\t: %s\r\n""Size\t: %d\r\n""Region\t: %s\r\n"
		"Mode\t: %s\r\n""Com\t: %s\r\n",
		szFile,
		Size,
		szRegion,
		((ModeFlag==BST_CHECKED)?"ForceLock":"default"),
		szCom
		);
	SetDlgItemText(hDlg_Download,IDC_ED_DOWNLOAD_INFO,Log);
	
	//Check RegionType
	int use_region;
	{
		target_context_t *tc = NULL;
		ret = CommandMemmap(szCom,&tc);
		if(ret == FALSE){
			SendMessage(hDlg_Download, WM_CLOSE, (WPARAM)0, (LPARAM)0);
			return FALSE;
		}
		
		use_region = Check_MemRegion(tc->memmap, szRegion);
		CommandFinish(&tc);
	}

	register_download_callback_function(Callback_Download);

	sprintf(Log,
		"Download START\r\n"
		"File\t: %s\r\n""Size\t: %d\r\n""Region\t: %s\r\n"
		"Mode\t: %s\r\n""Com\t: %s",
		szFile,
		Size,
		szRegion,
		((ModeFlag==BST_CHECKED)?"ForceLock":"default"),
		szCom
		);
	Display_Log(Log);

	if(use_region == TRUE){
		//region mode
		target_context_t *tc = NULL;
		ret = CommandDownload_Region(szCom,&tc,szFile,szRegion,ModeFlag);
		CommandFinish(&tc);
	}else{
		//address mode
		char *endptr;
		unsigned long addr;
		int length = strlen(szRegion);
		
		addr = strtoul(szRegion,&endptr,16);
		if(
			(endptr != (char *)((u_long)szRegion + (u_long)length)) ||
			(addr == ULONG_MAX)
			){
			MessageBox(hDlg_Download,"unknown adress","error",MB_OK);
			SendMessage(hDlg_Download, WM_CLOSE, (WPARAM)0, (LPARAM)0);
			return FALSE;
		}

		target_context_t *tc = NULL;
		ret = CommandDownload_Address(szCom,&tc,szFile,addr,ModeFlag);
		CommandFinish(&tc);
	}

	if(ret == TRUE){
		Display_Log("Download COMPLETE");
	}else{
		Display_Log("Download FAILED");
	}

	SendMessage(hDlg_Download, WM_CLOSE, (WPARAM)0, (LPARAM)0);
	return TRUE;
}