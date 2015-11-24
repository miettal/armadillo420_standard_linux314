
#include <windows.h>
#include <windowsx.h>
#include <commctrl.h>
#include <shlwapi.h>

#include <time.h>
#include <sys/types.h>
#include <sys/stat.h>

#include "resource.h"

#include "hermit.h"
#include "hermit_command.h"
#include "hermit_util.h"

void GetFileInfo(HWND hWnd, HWND nIDDlgItem){
    OPENFILENAME ofn;
	char szFile[256];
	
    memset(&ofn,0,sizeof(OPENFILENAME));
	memset(szFile,0,256);
	
    ofn.lStructSize=sizeof(OPENFILENAME);
    ofn.hwndOwner=hWnd;
    ofn.lpstrFilter="image files(*.bin;*.bin.gz;*.img;*.img.gz)\0*.bin;*.bin.gz;*.img;*.img.gz\0All files(*.*)\0*.*\0\0";
    ofn.lpstrFile=szFile;
    ofn.lpstrFileTitle=0;
    ofn.nMaxFile=MAX_PATH;
    ofn.Flags=OFN_FILEMUSTEXIST | OFN_HIDEREADONLY;
    ofn.lpstrDefExt="txt";
    ofn.lpstrTitle="ファイルを選択して下さい";
	
    if(GetOpenFileName(&ofn) == 0) return;
	Edit_SetText(nIDDlgItem,szFile);
	
    return;
}

int InitComboBox_SerialComm(HWND hWnd,int nIDDlgItem){
	HKEY		hKey;
	int			i;
	char		szName[MAX_PATH], szValue[MAX_PATH];
	DWORD		dwNameSize, dwValueSize, dwType;

	HINSTANCE hDll = LoadLibrary ("shlwapi.dll");
	if(!hDll) {
		MessageBox(NULL,"error","Can't load dll.",0);
		return FALSE;
	}
	PSHEnumValue SHEnumValue = (PSHEnumValue)GetProcAddress (hDll, "SHEnumValueA"); 
	if (!SHEnumValue) {
		MessageBox(NULL,"error","Bad dll.",0);
		return FALSE;
	}

	RegOpenKeyEx (HKEY_LOCAL_MACHINE, "Hardware\\DeviceMap\\SerialComm", 0, KEY_READ, &hKey);
	for (i = 0; ; i++)	{
		dwNameSize = sizeof (szName);
		dwValueSize = sizeof (szValue);
		if (ERROR_SUCCESS != SHEnumValue (hKey, i, szName, &dwNameSize, &dwType, szValue, &dwValueSize))	{
			break;
		}
		SendMessage(GetDlgItem(hWnd, nIDDlgItem), CB_INSERTSTRING, (WPARAM)i, (LPARAM)szValue);
	}
	SendMessage(GetDlgItem(hWnd, nIDDlgItem), CB_SETCURSEL, (WPARAM)0, (LPARAM)0);
	return TRUE;
}



int InitComboBox_MemRegion(HWND hWnd,int nIDDlgItem,const mem_region_t *mr){
	for(; mr; mr = mr->next){
		switch(mr->type){
		case MEMTYPE_FLASH:
		case MEMTYPE_RAM:
			SendMessage(GetDlgItem(hWnd,nIDDlgItem), CB_INSERTSTRING, (WPARAM)0, (LPARAM)mr->name);
			break;
		default:
			continue;
		}
	}
	return TRUE;
}

int Display_MemRegion(const mem_region_t *mr){
	char buf[4096];
	int ptr = 0;
	ptr += sprintf(&buf[ptr],"memregion");
	for(; mr; mr = mr->next){
		ptr += sprintf(&buf[ptr],"\r\n\t");
		switch(mr->type){
		case MEMTYPE_FLASH: ptr += sprintf(&buf[ptr], "FLASH: ");break;
		case MEMTYPE_RAM:   ptr += sprintf(&buf[ptr], "RAM  : ");break;
		case MEMTYPE_ROM:   ptr += sprintf(&buf[ptr], "ROM  : ");break;
		}
		ptr += sprintf(&buf[ptr],"0x%08x-0x%08x: %s %s",
					(size_t)mr->lma,(size_t)mr->lma+mr->size,
					mr->name,
					(mr->flags ? "(ForceLocked)":"")
					);
	}
	Display_Log(buf);
	//SetDlgItemText(hDlg_Main,IDC_MA_EB_LOG,buf);

	return TRUE;
}

int Check_MemRegion(const mem_region_t *mr,char *region){
	for(; mr; mr = mr->next){
		if(strcmp(mr->name,region) == 0){
			return TRUE;
		}
	}
	return FALSE;
}

int Display_Version(void){
	char buf[4096];
	int ptr = 0;
	char shoehorn_dll_version[4096] = "    Shoehorn.dll (unknown)";

	HMODULE hDll = LoadLibrary("libs/shoehorn.dll");
	if (hDll != NULL){
		char *tmp;
		char *(*dll_version)(void);
		dll_version = (char *(*)(void))GetProcAddress(hDll, "DllVersion");
		if (dll_version) {
			tmp = dll_version();
			strcpy(shoehorn_dll_version, tmp);
		}
		FreeLibrary(hDll);
	}

	//fix: COMMENT 06/03/07
	ptr += sprintf(&buf[ptr], "version\r\n");
	ptr += sprintf(&buf[ptr], HERMIT_WIN_VERSION);
	ptr += sprintf(&buf[ptr], "\r\n");
	ptr += sprintf(&buf[ptr], shoehorn_dll_version);
	Display_Log(buf);
	return TRUE;
}

int Display_Log(char *str){
	char log_file_path[256];
	static int initflag = 0;
	static FILE *fp = NULL;

	struct tm *nowtime;
	time_t ltime;

	while(fp){Sleep(100);};
	sprintf(log_file_path,"%s\\%s",AppDir,LOG_FILE_NAME);

	if(initflag == 0){
		fp = fopen(log_file_path,"w");
		SendMessage(GetDlgItem(hDlg_Main,IDC_MA_EB_LOG), EM_SETMARGINS, (WPARAM)EC_LEFTMARGIN | EC_RIGHTMARGIN, (LPARAM)MAKELONG(8,8));
	}else{
		fp = fopen(log_file_path,"a");
	}
	if(fp == NULL){
		SetDlgItemText(hDlg_Main,IDC_MA_EB_LOG,"log error...");
		initflag = 0;
		return FALSE;
	}else{
		initflag++;
	}
		
	time(&ltime);
	nowtime = localtime( &ltime );
	
	fprintf(fp,"%04d/%02d/%02d %02d:%02d:%02d: %s\r\n",
		nowtime->tm_year+1900,nowtime->tm_mon+1,nowtime->tm_mday,
		nowtime->tm_hour,nowtime->tm_min,nowtime->tm_sec,
		str);

	fclose(fp);fp = NULL;
	fp=fopen(log_file_path,"r");

	int ret;
	struct _stat filestatus;
	char *buf;

	ret = _stat(log_file_path,&filestatus);
	if(ret == -1) return FALSE;

	buf = (char *)calloc(1,filestatus.st_size+1);
	if(buf == NULL) return FALSE;

	fread(buf,filestatus.st_size,1,fp);
	fclose(fp);fp = NULL;

	SetDlgItemText(hDlg_Main,IDC_MA_EB_LOG,buf);
	free(buf);

	int line_count;
	line_count = SendMessage(GetDlgItem(hDlg_Main,IDC_MA_EB_LOG), EM_GETLINECOUNT, (WPARAM)0, (LPARAM)0);
	SendMessage(GetDlgItem(hDlg_Main,IDC_MA_EB_LOG), EM_LINESCROLL, (WPARAM)0, (LPARAM)line_count);

	return TRUE;
}

int Display_LastError(char *str){
	char buf[512];
	LPTSTR lpMsgBuf;
	
	FormatMessage( 
		FORMAT_MESSAGE_ALLOCATE_BUFFER | 
		FORMAT_MESSAGE_FROM_SYSTEM | 
		FORMAT_MESSAGE_IGNORE_INSERTS,
		NULL,
		GetLastError(),
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // デフォルト言語
		(LPTSTR) &lpMsgBuf,
		0,
		NULL 
		);

	int i;
	for(i=strlen(lpMsgBuf);i!=0;i--){
		if(lpMsgBuf[i] == '\n'){
			lpMsgBuf[i] = 0;
			break;
		}
	}

	sprintf(buf, "%s: %s", str, lpMsgBuf);
	LocalFree((LPVOID)lpMsgBuf);

	Display_Log(buf);
	MessageBox( NULL, buf, "Error", MB_OK | MB_ICONINFORMATION );
	
	return TRUE;
}