# Microsoft Developer Studio Project File - Name="ConfigTool" - Package Owner=<4>
# Generated nmake makefile based on ConfigTool.dsp.

!IF "$(CFG)" == ""
CFG=ConfigTool - Win32 Debug
!ENDIF

!IF "$(CFG)" != "ConfigTool - Win32 Release" && "$(CFG)" != "ConfigTool - Win32 Debug"
!ERROR Invalid CFG "$(CFG)". Use "ConfigTool - Win32 Release" or "ConfigTool - Win32 Debug".
!ENDIF

!IF "$(VC6_ROOT)" != ""
CPP="$(VC6_ROOT)\Bin\cl.exe"
RSC="$(VC6_ROOT)\Bin\rc.exe"
LINK32="$(VC6_ROOT)\Bin\link.exe"

VC6_INC=/I "$(VC6_ROOT)\ATL\Include" /I "$(VC6_ROOT)\Include" /I "$(VC6_ROOT)\MFC\Include"
VC6_LIB=/libpath:"$(VC6_ROOT)\Lib" /libpath:"$(VC6_ROOT)\MFC\Lib"
!ELSE
CPP=cl.exe
RSC=rc.exe
LINK32=link.exe
VC6_INC=
VC6_LIB=
!ENDIF

LOCAL_INC=/I "src" /I "include"

!IF "$(VIRTOOLS_SDK_PATH)" != ""
VIRTOOLS_INC=/I "$(VIRTOOLS_SDK_PATH)\Include" /I "$(VIRTOOLS_SDK_PATH)\Includes" /I "$(VIRTOOLS_SDK_PATH)\include"
VIRTOOLS_LIB=/libpath:"$(VIRTOOLS_SDK_PATH)\Lib" /libpath:"$(VIRTOOLS_SDK_PATH)\lib"
!ELSE
VIRTOOLS_INC=
VIRTOOLS_LIB=
!ENDIF

OUTDIR=Bin

!IF "$(CFG)" == "ConfigTool - Win32 Release"
CFGDIR=Release
INTDIR=Release\ConfigTool
CPP_PROJ=/nologo /W3 /GX /O2 $(LOCAL_INC) $(VIRTOOLS_INC) $(VC6_INC) /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "UNICODE" /D "_UNICODE" /D "CONFIGTOOL_STANDALONE" /FD /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /c
RSC_PROJ=$(VC6_INC) $(LOCAL_INC) $(VIRTOOLS_INC) /l 0x409 /fo"$(INTDIR)\ConfigTool.res" /d "NDEBUG"
LINK32_FLAGS=VxMath.lib kernel32.lib user32.lib gdi32.lib shell32.lib /nologo /subsystem:windows /machine:I386 /out:"$(OUTDIR)\ConfigTool.exe" $(VC6_LIB) $(VIRTOOLS_LIB)
!ELSE
CFGDIR=Debug
INTDIR=Debug\ConfigTool
CPP_PROJ=/nologo /W3 /Gm /GX /ZI /Od $(LOCAL_INC) $(VIRTOOLS_INC) $(VC6_INC) /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "UNICODE" /D "_UNICODE" /D "CONFIGTOOL_STANDALONE" /FR"$(INTDIR)\\" /FD /GZ /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /c
RSC_PROJ=$(VC6_INC) $(LOCAL_INC) $(VIRTOOLS_INC) /l 0x409 /fo"$(INTDIR)\ConfigTool.res" /d "_DEBUG"
LINK32_FLAGS=VxMath.lib kernel32.lib user32.lib gdi32.lib shell32.lib /nologo /subsystem:windows /debug /machine:I386 /out:"$(OUTDIR)\ConfigTool.exe" /pdbtype:sept $(VC6_LIB) $(VIRTOOLS_LIB)
!ENDIF

ALL : "$(OUTDIR)\ConfigTool.exe"

CLEAN :
	-@if exist "$(INTDIR)\*.obj" del /q "$(INTDIR)\*.obj"
	-@if exist "$(INTDIR)\*.res" del /q "$(INTDIR)\*.res"
	-@if exist "$(INTDIR)\*.sbr" del /q "$(INTDIR)\*.sbr"
	-@if exist "$(INTDIR)\*.idb" del /q "$(INTDIR)\*.idb"
	-@if exist "$(INTDIR)\*.pdb" del /q "$(INTDIR)\*.pdb"
	-@if exist "$(OUTDIR)\ConfigTool.exe" del /q "$(OUTDIR)\ConfigTool.exe"
	-@if exist "$(OUTDIR)\ConfigTool.pdb" del /q "$(OUTDIR)\ConfigTool.pdb"
	-@if exist ".\src\ConfigToolVersion.vc6.rc" del /q ".\src\ConfigToolVersion.vc6.rc"

"$(OUTDIR)" :
	@if not exist "$(OUTDIR)\$(NULL)" mkdir "$(OUTDIR)"

"$(CFGDIR)" :
	@if not exist "$(CFGDIR)\$(NULL)" mkdir "$(CFGDIR)"

"$(INTDIR)" : "$(CFGDIR)"
	@if not exist "$(INTDIR)\$(NULL)" mkdir "$(INTDIR)"

OBJS= \
	"$(INTDIR)\CmdlineParser.obj" \
	"$(INTDIR)\ConfigTool.obj" \
	"$(INTDIR)\GameConfig.obj" \
	"$(INTDIR)\Utils.obj" \
	"$(INTDIR)\ConfigTool.res"

"$(OUTDIR)\ConfigTool.exe" : "$(OUTDIR)" "$(INTDIR)" $(OBJS)
	$(LINK32) @<<
$(LINK32_FLAGS) $(OBJS)
<<

"$(INTDIR)\CmdlineParser.obj" : ".\src\CmdlineParser.cpp" "$(INTDIR)"
	$(CPP) $(CPP_PROJ) ".\src\CmdlineParser.cpp"

"$(INTDIR)\ConfigTool.obj" : ".\src\ConfigTool.cpp" "$(INTDIR)"
	$(CPP) $(CPP_PROJ) ".\src\ConfigTool.cpp"

"$(INTDIR)\GameConfig.obj" : ".\src\GameConfig.cpp" "$(INTDIR)"
	$(CPP) $(CPP_PROJ) ".\src\GameConfig.cpp"

"$(INTDIR)\Utils.obj" : ".\src\Utils.cpp" "$(INTDIR)"
	$(CPP) $(CPP_PROJ) ".\src\Utils.cpp"

"$(INTDIR)\ConfigTool.res" : ".\src\ConfigTool.rc" ".\src\ConfigToolVersion.rc.in" ".\src\ConfigToolVersion.vc6.rc" "$(INTDIR)"
	$(RSC) $(RSC_PROJ) ".\src\ConfigTool.rc"

".\src\ConfigToolVersion.vc6.rc" : ".\src\ConfigToolVersion.rc.in" ".\VERSION"
	powershell -NoProfile -ExecutionPolicy Bypass -File ".\scripts\generate-player-version-rc.ps1" -Template ".\src\ConfigToolVersion.rc.in" -Output ".\src\ConfigToolVersion.vc6.rc" -VersionFile ".\VERSION"
