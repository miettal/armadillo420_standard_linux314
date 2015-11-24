
#ifndef _HERMIT_UTIL_H_
#define _HERMIT_UTIL_H_

#include <windows.h>
#include <host/common/target.h>
#include <host/common/memmap.h>

typedef long (WINAPI *PSHEnumValue)(HKEY hkey, DWORD dwIndex, LPSTR pszValueName, LPDWORD pcchValueName, LPDWORD pdwType, LPVOID pvData, LPDWORD pcbData); 


void GetFileInfo(HWND hWnd, HWND nIDDlgItem);

int InitComboBox_SerialComm(HWND hWnd,int nIDDlgItem);
int InitComboBox_MemRegion(HWND hWnd,int nIDDlgItem,const mem_region_t *mr);
int Display_MemRegion(const mem_region_t *mr);
int Check_MemRegion(const mem_region_t *mr,char *region);
int Display_Version(void);
int Display_Log(char *str);
int Display_LastError(char *str);
#endif