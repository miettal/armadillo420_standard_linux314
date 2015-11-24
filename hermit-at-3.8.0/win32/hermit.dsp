# Microsoft Developer Studio Project File - Name="hermit" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** 編集しないでください **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=hermit - Win32 Debug
!MESSAGE これは有効なﾒｲｸﾌｧｲﾙではありません。 このﾌﾟﾛｼﾞｪｸﾄをﾋﾞﾙﾄﾞするためには NMAKE を使用してください。
!MESSAGE [ﾒｲｸﾌｧｲﾙのｴｸｽﾎﾟｰﾄ] ｺﾏﾝﾄﾞを使用して実行してください
!MESSAGE 
!MESSAGE NMAKE /f "hermit.mak".
!MESSAGE 
!MESSAGE NMAKE の実行時に構成を指定できます
!MESSAGE ｺﾏﾝﾄﾞ ﾗｲﾝ上でﾏｸﾛの設定を定義します。例:
!MESSAGE 
!MESSAGE NMAKE /f "hermit.mak" CFG="hermit - Win32 Debug"
!MESSAGE 
!MESSAGE 選択可能なﾋﾞﾙﾄﾞ ﾓｰﾄﾞ:
!MESSAGE 
!MESSAGE "hermit - Win32 Release" ("Win32 (x86) Application" 用)
!MESSAGE "hermit - Win32 Debug" ("Win32 (x86) Application" 用)
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "hermit - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /YX /FD /c
# ADD CPP /nologo /W3 /GX /O2 /I "../src" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /FR /YX /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x411 /d "NDEBUG"
# ADD RSC /l 0x411 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib comctl32.lib /nologo /subsystem:windows /machine:I386

!ELSEIF  "$(CFG)" == "hermit - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /YX /FD /GZ /c
# ADD CPP /nologo /W3 /Gm /GX /ZI /Od /I "../src" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /FR /YX /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x411 /d "_DEBUG"
# ADD RSC /l 0x411 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib comctl32.lib /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept

!ENDIF 

# Begin Target

# Name "hermit - Win32 Release"
# Name "hermit - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;"
# Begin Source File

SOURCE=..\src\host\common\console.c
# End Source File
# Begin Source File

SOURCE=..\src\host\common\crc.c
# End Source File
# Begin Source File

SOURCE=..\src\host\common\download.c
# End Source File
# Begin Source File

SOURCE=..\src\host\common\erase.c
# End Source File
# Begin Source File

SOURCE=..\src\host\common\flash.c
# End Source File
# Begin Source File

SOURCE=.\hermit.cpp
# End Source File
# Begin Source File

SOURCE=.\hermit_command.cpp
# End Source File
# Begin Source File

SOURCE=.\hermit_util.cpp
# End Source File
# Begin Source File

SOURCE=..\src\host\common\memmap.c
# End Source File
# Begin Source File

SOURCE=..\src\host\common\options.c
# End Source File
# Begin Source File

SOURCE=..\src\host\common\region.c
# End Source File
# Begin Source File

SOURCE=..\src\host\common\serial.c
# End Source File
# Begin Source File

SOURCE=.\sheet_download.cpp
# End Source File
# Begin Source File

SOURCE=.\sheet_erase.cpp
# End Source File
# Begin Source File

SOURCE=.\sheet_shoehorn.cpp
# End Source File
# Begin Source File

SOURCE=..\src\host\common\target.c
# End Source File
# Begin Source File

SOURCE=..\src\host\common\util.c
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h"
# Begin Source File

SOURCE=..\src\host\common\console.h
# End Source File
# Begin Source File

SOURCE=..\src\host\common\crc.h
# End Source File
# Begin Source File

SOURCE=..\src\host\common\download.h
# End Source File
# Begin Source File

SOURCE=..\src\host\common\erase.h
# End Source File
# Begin Source File

SOURCE=..\src\host\common\flash.h
# End Source File
# Begin Source File

SOURCE=.\hermit.h
# End Source File
# Begin Source File

SOURCE=.\hermit_command.h
# End Source File
# Begin Source File

SOURCE=.\hermit_util.h
# End Source File
# Begin Source File

SOURCE=..\src\host\common\main.h
# End Source File
# Begin Source File

SOURCE=..\src\host\common\memmap.h
# End Source File
# Begin Source File

SOURCE=..\src\host\common\options.h
# End Source File
# Begin Source File

SOURCE=..\src\host\common\region.h
# End Source File
# Begin Source File

SOURCE=..\src\host\common\serial.h
# End Source File
# Begin Source File

SOURCE=.\sheet_download.h
# End Source File
# Begin Source File

SOURCE=.\sheet_erase.h
# End Source File
# Begin Source File

SOURCE=.\sheet_shoehorn.h
# End Source File
# Begin Source File

SOURCE=..\src\host\common\target.h
# End Source File
# Begin Source File

SOURCE=..\src\host\common\ttype.h
# End Source File
# Begin Source File

SOURCE=..\src\host\common\util.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;rc"
# Begin Source File

SOURCE=.\hermit.ico
# End Source File
# Begin Source File

SOURCE=.\hermit.rc
# End Source File
# End Group
# End Target
# End Project
