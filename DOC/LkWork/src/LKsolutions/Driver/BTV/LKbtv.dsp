# Microsoft Developer Studio Project File - Name="LKbtv" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=LKbtv - Win32 Release
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "LKbtv.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "LKbtv.mak" CFG="LKbtv - Win32 Release"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "LKbtv - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "LKbtv - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "LKbtv - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "NDEBUG" /YX /FD /c
# ADD CPP /nologo /Gz /W3 /O2 /I "\NTDDK\inc" /D "NDEBUG" /D "_X86_" /D _WIN32_WINNT=0x400 /YX /FD -GF /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /machine:I386
# ADD LINK32 int64.lib ntoskrnl.lib hal.lib /nologo /base:"0x10000" /entry:"DriverEntry" /pdb:none /machine:I386 /nodefaultlib /out:"Release\LKbtv.SYS" /libpath:"D:\NTDDK\libfre\i386" -driver -subsystem:NATIVE,4.00

!ELSEIF  "$(CFG)" == "LKbtv - Win32 Debug"

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
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "_DEBUG" /YX /FD /GZ /c
# ADD CPP /nologo /Gz /ML /W3 /Gm /Gi /Zi /Od /I "D:\NTDDK\inc\ddk" /I "D:\NTDDK\inc" /D "_DEBUG" /D "_X86_" /D _WIN32_WINNT=0x500 /FR /FD /Gs -GF /c
# SUBTRACT CPP /Gy /YX /Yc /Yu
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept
# ADD LINK32 libc.lib ntoskrnl.lib hal.lib int64.lib LkBinutild.lib /nologo /base:"0x10000" /entry:"DriverEntry" /incremental:no /map /debug /machine:I386 /nodefaultlib /out:"Debug\LKbtv.SYS" /libpath:"D:\NTDDK\libchk\i386" -driver -subsystem:NATIVE,4.00
# Begin Special Build Tool
SOURCE="$(InputPath)"
PostBuild_Desc=Copy Driver to System32
PostBuild_Cmds=copy debug\LKbtv.SYS $(SystemRoot)\System32\Drivers\*.*	e:\lkwork\bin\nmsym /trans:source,package,always debug\LKbtv.SYS	copy debug\LKbtv.nms $(SystemRoot)\System32\Drivers\*.*	e:\lkwork\bin\nmsym /sym:debug\LKbtv.nms
# End Special Build Tool

!ENDIF 

# Begin Target

# Name "LKbtv - Win32 Release"
# Name "LKbtv - Win32 Debug"
# Begin Group "GENERIC"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\generic\Control.cpp
# End Source File
# Begin Source File

SOURCE=.\generic\DevQueue.cpp
# End Source File
# Begin Source File

SOURCE=.\generic\Driver.h
# End Source File
# Begin Source File

SOURCE=.\generic\DriverEntry.cpp
# End Source File
# Begin Source File

SOURCE=.\generic\Generic.h
# End Source File
# Begin Source File

SOURCE=.\generic\GenericPower.h
# End Source File
# Begin Source File

SOURCE=.\generic\PlugPlay.cpp
# End Source File
# Begin Source File

SOURCE=.\generic\Power.cpp
# End Source File
# Begin Source File

SOURCE=.\generic\RemoveLock.cpp
# End Source File
# Begin Source File

SOURCE=.\generic\stddcls.cpp
# End Source File
# Begin Source File

SOURCE=.\generic\stddcls.h
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\ioctl.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# End Group
# Begin Group "MAIN"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\driver.cpp
# End Source File
# Begin Source File

SOURCE=.\Driver.h
# End Source File
# Begin Source File

SOURCE=.\hardware.cpp
# End Source File
# Begin Source File

SOURCE=.\hardware.h
# End Source File
# Begin Source File

SOURCE=.\Unicode.cpp
# End Source File
# Begin Source File

SOURCE=.\Unicode.h
# End Source File
# End Group
# End Target
# End Project
