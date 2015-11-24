#ifndef _HERMIT_H_
#define _HERMIT_H_

#define HERMIT_WIN_VERSION ( \
	"    Hermit-At WIN32 v1.3.0\r\n" \
	"                    Copyright (C) 2005-2010 Atmark Techno, Inc.\r\n" \
	"        * support ERASE command\r\n")
							
extern HWND hDlg_Main;
extern HWND hDlg_Download;
extern HWND hDlg_Version;

#define APP_DIR_LEN (OFS_MAXPATHNAME + 1)
extern char AppDir[APP_DIR_LEN];


#define LOG_FILE_NAME "hermit.log"

typedef enum _sheet_id_e{
	SheetID_Download = 0,
	SheetID_Erase,
	SheetID_Memmap,
	SheetID_Shoehorn,
	SheetID_Version,
}sheet_id_e;

typedef LRESULT (CALLBACK *TabSheetProcFunc)(HWND hWnd, UINT msg, WPARAM wp, LPARAM lp);

#define SheetName_Download "Download"
#define SheetName_Erase    "Erase"
#define SheetName_Memmap   "Memmap"
#define SheetName_Shoehorn "Shoehorn"
#define SheetName_Version  "Version"

struct TabControlSheetInfo{
	HWND hWnd; 
	int sheet;
	LPTSTR title;
	int id;
	TabSheetProcFunc proc;
};

#endif
