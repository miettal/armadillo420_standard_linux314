#include <windows.h>
#include <windowsx.h>

#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>

#include "hermit.h"
#include "resource.h"
#include "hermit_util.h"
#include "sheet_shoehorn.h"

#define SHOEHORN_LOG_FILE_NAME "shoehorn.log"

HWND hDlg_Shoehorn;
HWND hSheetShoehorn;
DWORD ShoehornThreadID;
HANDLE hShoehornThread;

static int ShoehornFunc(HWND hWnd);
DWORD WINAPI ShoehornThread(LPVOID arg);

struct sh_target_info {
	char name[256];
	char loader[256];
	char bin[256];
	char init[256];
	char post[256];
	struct sh_target_info *next;
};

static struct sh_target_info *_target_info_head = NULL;

static int add_sh_target(char *name, char *loader, char *bin, char *init, char *post){
	struct sh_target_info *info;
	info = (struct sh_target_info *)malloc(sizeof(struct sh_target_info));

	strncpy(info->name, name, 256);
	strncpy(info->loader, loader, 256);
	strncpy(info->bin , bin , 256);
	strncpy(info->init, init, 256);
	strncpy(info->post, post, 256);
	info->next = NULL;

	if(_target_info_head == NULL){
		_target_info_head = info;
	}else{
		struct sh_target_info *curinfo = _target_info_head;
		for(;curinfo->next != NULL;curinfo = curinfo->next);
		curinfo->next = info;
	}
	return TRUE;
}

static int get_sh_target_info(char *name, struct sh_target_info *info){
	struct sh_target_info *cur;

	cur = _target_info_head;
	while(cur != NULL){
		if(_stricmp(cur->name, name) == 0){
			memcpy(info, cur, sizeof(struct sh_target_info));
			info->next = 0;
			return TRUE;
		}
		cur = cur->next;
	}
	return FALSE;
}

static int SearchShoehornTarget(HWND hWnd){
	HANDLE hFile = INVALID_HANDLE_VALUE;
    WIN32_FIND_DATA data;
	int index = 0;

	SendMessage(GetDlgItem(hWnd, IDC_SH_CB_TARGET), CB_RESETCONTENT, (WPARAM)0, (LPARAM)0);

    hFile = FindFirstFile("libs/shoehorn-*.bin", &data);
    if( hFile != INVALID_HANDLE_VALUE )
    {
        do{
			char buf[256];
			char name[256], loader[256], bin[256], init[256], post[256];
			char *tmp_start, *tmp_end;
			int flag;
			
			flag = 0;

			tmp_start = strchr(data.cFileName, '-') + 1;
			tmp_end   = strrchr(tmp_start, '.');
			strncpy(name, tmp_start, tmp_end - tmp_start);
			name[tmp_end - tmp_start] = 0;
			
			_snprintf(bin, 256, "libs/%s", data.cFileName);
			//strncpy(bin, data.cFileName, strlen(data.cFileName)+1);
			
			HANDLE hInitFile = INVALID_HANDLE_VALUE;
			sprintf(buf, "libs/shoehorn-%s.init", name);
			hInitFile = FindFirstFile(buf, &data);
			if(hInitFile != INVALID_HANDLE_VALUE){
				_snprintf(init, 256, "libs/%s", data.cFileName);
				//strncpy(init, data.cFileName, strlen(data.cFileName)+1);
				FindClose(hInitFile);
				flag ++;
			}
			
			HANDLE hPostFile = INVALID_HANDLE_VALUE;
			sprintf(buf, "libs/shoehorn-%s.post", name);
			hPostFile = FindFirstFile(buf, &data);
			if(hPostFile != INVALID_HANDLE_VALUE){
				_snprintf(post, 256, "libs/%s", data.cFileName);
				//strncpy(post, data.cFileName, strlen(data.cFileName)+1);
				FindClose(hPostFile);
				flag ++;
			}

			HANDLE hLoaderFile = INVALID_HANDLE_VALUE;
			sprintf(buf, "libs/loader-%s-boot.bin", name);
			hLoaderFile = FindFirstFile(buf, &data);
			if(hLoaderFile != INVALID_HANDLE_VALUE){
				_snprintf(loader, 256, "libs/%s", data.cFileName);
				//strncpy(loader, data.cFileName, strlen(data.cFileName)+1);
				FindClose(hLoaderFile);
				flag ++;
			}
			
			if(flag == 3){
				add_sh_target(name, loader, bin, init, post);
				SendMessage(GetDlgItem(hWnd, IDC_SH_CB_TARGET), CB_INSERTSTRING, (WPARAM)index++, (LPARAM)name);
			}

        }while( FindNextFile( hFile, &data) );
		
        FindClose( hFile );
    }

	SendMessage(GetDlgItem(hWnd, IDC_SH_CB_TARGET), CB_SETCURSEL, (WPARAM)0, (LPARAM)0);
	return TRUE;
}

LRESULT CALLBACK TabSheetProc_Shoehorn(HWND hWnd, UINT msg, WPARAM wp, LPARAM lp){
	switch(msg){
	case WM_INITDIALOG:

		SearchShoehornTarget(hWnd);

		return TRUE;
	case WM_SHOWWINDOW:
		SetCurrentDirectory(AppDir);
		return TRUE;
	case WM_COMMAND:
		switch (GET_WM_COMMAND_ID(wp, lp)) {
		case IDC_SH_BT_ACTION:
			ShoehornFunc(hWnd);
			break;
		}
		break;
	}
	return FALSE;
}

LRESULT CALLBACK DlgProc_Shoehorn(HWND hWnd, UINT msg, WPARAM wp, LPARAM lp)
{
    switch (msg) {
	case WM_INITDIALOG:
		hDlg_Shoehorn=hWnd;
		return TRUE;

	case WM_COMMAND:
		switch (GET_WM_COMMAND_ID(wp, lp)) {
		case IDCANCEL:
			
			HMODULE hDll;
			
			void (*shoehorn_cancel)(void);
			
			hDll = LoadLibrary("libs/shoehorn.dll");
			if(hDll == NULL){
				Display_LastError("libs/shoehorn.dll");
				return FALSE;
			}
			
			shoehorn_cancel = (void (*)(void))GetProcAddress(hDll, "ShoehornCancel");
			shoehorn_cancel();

			FreeLibrary(hDll);
			
			return TRUE;
		}
		break;

	case WM_CLOSE:
		EndDialog(hWnd, IDOK);
		return TRUE;
	}
	return FALSE;
}

static int ShoehornFunc(HWND hWnd){
	char log_file_path[256];
	FILE *fp;
	sprintf(log_file_path,"%s\\%s",AppDir,SHOEHORN_LOG_FILE_NAME);
	fp = fopen(log_file_path, "w");
	fclose(fp);

	hShoehornThread = CreateThread(NULL,0,ShoehornThread,hWnd,0,&ShoehornThreadID);
	DialogBox((HINSTANCE)GetWindowLong(hWnd,GWL_HINSTANCE), "DLG_SHOEHORN",hWnd, (DLGPROC)DlgProc_Shoehorn);
	
	return FALSE;
}



int Callback_Message(char *str){
	char log_file_path[256];
	static int initflag = 0;
	static FILE *fp = NULL;

	while(fp){Sleep(100);};
	sprintf(log_file_path,"%s\\%s",AppDir,SHOEHORN_LOG_FILE_NAME);

	if(initflag == 0){
		fp = fopen(log_file_path,"w");
		SendMessage(GetDlgItem(hDlg_Shoehorn,IDC_ED_SHOEHORN_INFO), EM_SETMARGINS, (WPARAM)EC_LEFTMARGIN | EC_RIGHTMARGIN, (LPARAM)MAKELONG(8,8));
	}else{
		fp = fopen(log_file_path,"a");
	}
	if(fp == NULL){
		Display_Log("log error...");
		initflag = 0;
		return FALSE;
	}else{
		initflag++;
	}
		
	fprintf(fp,"%s",str);

	fclose(fp);fp = NULL;
	fp=fopen(log_file_path,"rb");

	int ret;
	struct _stat filestatus;
	char *buf;

	ret = _stat(log_file_path,&filestatus);
	if(ret == -1) return FALSE;

	buf = (char *)calloc(1,filestatus.st_size+1);
	if(buf == NULL) return FALSE;

	fread(buf,1,filestatus.st_size,fp);
	fclose(fp);fp = NULL;

	SetDlgItemText(hDlg_Shoehorn,IDC_ED_SHOEHORN_INFO,buf);
	free(buf);

	int line_count;
	line_count = SendMessage(GetDlgItem(hDlg_Shoehorn,IDC_ED_SHOEHORN_INFO), EM_GETLINECOUNT, (WPARAM)0, (LPARAM)0);
	SendMessage(GetDlgItem(hDlg_Shoehorn,IDC_ED_SHOEHORN_INFO), EM_LINESCROLL, (WPARAM)0, (LPARAM)line_count);

	return TRUE;
}

void Callback_MessageBox(char *str){
	MessageBox(hDlg_Shoehorn, str, "callback", 0);
}

void Callback_Start(void){
	EnableWindow(GetDlgItem(hDlg_Shoehorn, IDCANCEL), FALSE);
}

DWORD WINAPI ShoehornThread(LPVOID arg){
	HWND hWnd = (HWND)arg;
	HMODULE hDll;
	void (*shoehorn_func)(char *,char *,char *,char*,char*,char*);
	void (*register_message)(void *);
	void (*register_messagebox)(void *);
	void (*register_start)(void *);

	char target[256];
	if(GetDlgItemText(hWnd, IDC_SH_CB_TARGET, target, 256) == 0){
		MessageBox(hWnd,"FILE","warning",MB_OK);
		SendMessage(hDlg_Shoehorn, WM_CLOSE, (WPARAM)0, (LPARAM)0);
		return FALSE;
	}
	char com[256];
	if(GetDlgItemText(hDlg_Main,IDC_MA_CB_COM,com,256)==0){
		MessageBox(hWnd,"COM","warning",MB_OK);
		SendMessage(hDlg_Shoehorn, WM_CLOSE, (WPARAM)0, (LPARAM)0);
		return FALSE;
	}



	struct sh_target_info target_info;
	int ret;
	ret = get_sh_target_info(target, &target_info);
	if(ret != TRUE){
		MessageBox(hWnd, "unknown target", "error", 0);
		SendMessage(hDlg_Shoehorn, WM_CLOSE, (WPARAM)0, (LPARAM)0);
		return FALSE;
	}

	hDll = LoadLibrary("libs/shoehorn.dll");
	if(hDll == NULL){
		Display_LastError("libs/shoehorn.dll");
		SendMessage(hDlg_Shoehorn, WM_CLOSE, (WPARAM)0, (LPARAM)0);
		return FALSE;
	}
	
	shoehorn_func = (void (*)(char *,char *,char *,char*,char*,char*))GetProcAddress (hDll, "ShoehornStart");
	register_message = (void (*)(void *))GetProcAddress(hDll, "RegisterFunc_Message");
	register_messagebox = (void (*)(void *))GetProcAddress(hDll, "RegisterFunc_MessageBox");
	register_start = (void (*)(void *))GetProcAddress(hDll, "RegisterFunc_Start");
	
	register_message(Callback_Message);
	register_messagebox(Callback_MessageBox);
	register_start(Callback_Start);
	
	shoehorn_func(com, target_info.name, target_info.loader, target_info.bin, target_info.init, target_info.post);
	
	SendMessage(hDlg_Shoehorn, WM_CLOSE, (WPARAM)0, (LPARAM)0);

	FreeLibrary(hDll);
	return TRUE;
}