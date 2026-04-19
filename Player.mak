# Microsoft Developer Studio Project File - Name="Player" - Package Owner=<4>
# Generated nmake makefile based on Player.dsp.

!IF "$(CFG)" == ""
CFG=Player - Win32 Debug
!ENDIF

!IF "$(CFG)" != "Player - Win32 Release" && "$(CFG)" != "Player - Win32 Debug"
!ERROR Invalid CFG "$(CFG)". Use "Player - Win32 Release" or "Player - Win32 Debug".
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

!IF "$(CFG)" == "Player - Win32 Release"
CFGDIR=Release
INTDIR=Release\Player
CPP_PROJ=/nologo /W3 /GX /O2 $(LOCAL_INC) $(VIRTOOLS_INC) $(VC6_INC) /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "UNICODE" /D "_UNICODE" /FD /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /c
RSC_PROJ=$(VC6_INC) $(LOCAL_INC) $(VIRTOOLS_INC) /l 0x409 /fo"$(INTDIR)\Player.res" /d "NDEBUG"
LINK32_FLAGS=CK2.lib VxMath.lib kernel32.lib user32.lib gdi32.lib shell32.lib delayimp.lib /nologo /subsystem:windows /machine:I386 /out:"$(OUTDIR)\Player.exe" /delayload:CK2.dll /delayload:VxMath.dll $(VC6_LIB) $(VIRTOOLS_LIB)
!ELSE
CFGDIR=Debug
INTDIR=Debug\Player
CPP_PROJ=/nologo /W3 /Gm /GX /ZI /Od $(LOCAL_INC) $(VIRTOOLS_INC) $(VC6_INC) /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "UNICODE" /D "_UNICODE" /FR"$(INTDIR)\\" /FD /GZ /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /c
RSC_PROJ=$(VC6_INC) $(LOCAL_INC) $(VIRTOOLS_INC) /l 0x409 /fo"$(INTDIR)\Player.res" /d "_DEBUG"
LINK32_FLAGS=CK2.lib VxMath.lib kernel32.lib user32.lib gdi32.lib shell32.lib delayimp.lib /nologo /subsystem:windows /debug /machine:I386 /out:"$(OUTDIR)\Player.exe" /pdbtype:sept /delayload:CK2.dll /delayload:VxMath.dll $(VC6_LIB) $(VIRTOOLS_LIB)
!ENDIF

ALL : "$(OUTDIR)\Player.exe" "$(OUTDIR)\ConfigTool.bat" "$(OUTDIR)\ConfigTool.vbs"

CLEAN :
	-@if exist "$(INTDIR)\*.obj" del /q "$(INTDIR)\*.obj"
	-@if exist "$(INTDIR)\*.res" del /q "$(INTDIR)\*.res"
	-@if exist "$(INTDIR)\*.sbr" del /q "$(INTDIR)\*.sbr"
	-@if exist "$(INTDIR)\*.idb" del /q "$(INTDIR)\*.idb"
	-@if exist "$(INTDIR)\*.pdb" del /q "$(INTDIR)\*.pdb"
	-@if exist "$(OUTDIR)\Player.exe" del /q "$(OUTDIR)\Player.exe"
	-@if exist "$(OUTDIR)\Player.pdb" del /q "$(OUTDIR)\Player.pdb"
	-@if exist ".\src\PlayerVersion.vc6.rc" del /q ".\src\PlayerVersion.vc6.rc"

"$(OUTDIR)" :
	@if not exist "$(OUTDIR)\$(NULL)" mkdir "$(OUTDIR)"

"$(CFGDIR)" :
	@if not exist "$(CFGDIR)\$(NULL)" mkdir "$(CFGDIR)"

"$(INTDIR)" : "$(CFGDIR)"
	@if not exist "$(INTDIR)\$(NULL)" mkdir "$(INTDIR)"

OBJS= \
	"$(INTDIR)\CmdlineParser.obj" \
	"$(INTDIR)\ConfigDialog.obj" \
	"$(INTDIR)\GameConfig.obj" \
	"$(INTDIR)\GamePlayer.obj" \
	"$(INTDIR)\Hotfix.obj" \
	"$(INTDIR)\Logger.obj" \
	"$(INTDIR)\Player.obj" \
	"$(INTDIR)\PlayerOptions.obj" \
	"$(INTDIR)\Splash.obj" \
	"$(INTDIR)\Utils.obj" \
	"$(INTDIR)\Player.res"

"$(OUTDIR)\Player.exe" : "$(OUTDIR)" "$(INTDIR)" $(OBJS)
	$(LINK32) @<<
$(LINK32_FLAGS) $(OBJS)
<<

"$(OUTDIR)\ConfigTool.bat" : ".\src\ConfigTool.bat" "$(OUTDIR)"
	copy /Y ".\src\ConfigTool.bat" "$(OUTDIR)\ConfigTool.bat"

"$(OUTDIR)\ConfigTool.vbs" : ".\src\ConfigTool.vbs" "$(OUTDIR)"
	copy /Y ".\src\ConfigTool.vbs" "$(OUTDIR)\ConfigTool.vbs"

"$(INTDIR)\CmdlineParser.obj" : ".\src\CmdlineParser.cpp" "$(INTDIR)"
	$(CPP) $(CPP_PROJ) ".\src\CmdlineParser.cpp"

"$(INTDIR)\ConfigDialog.obj" : ".\src\ConfigDialog.cpp" "$(INTDIR)"
	$(CPP) $(CPP_PROJ) ".\src\ConfigDialog.cpp"

"$(INTDIR)\GameConfig.obj" : ".\src\GameConfig.cpp" "$(INTDIR)"
	$(CPP) $(CPP_PROJ) ".\src\GameConfig.cpp"

"$(INTDIR)\GamePlayer.obj" : ".\src\GamePlayer.cpp" "$(INTDIR)"
	$(CPP) $(CPP_PROJ) ".\src\GamePlayer.cpp"

"$(INTDIR)\Hotfix.obj" : ".\src\Hotfix.cpp" "$(INTDIR)"
	$(CPP) $(CPP_PROJ) ".\src\Hotfix.cpp"

"$(INTDIR)\Logger.obj" : ".\src\Logger.cpp" "$(INTDIR)"
	$(CPP) $(CPP_PROJ) ".\src\Logger.cpp"

"$(INTDIR)\Player.obj" : ".\src\Player.cpp" "$(INTDIR)"
	$(CPP) $(CPP_PROJ) ".\src\Player.cpp"

"$(INTDIR)\PlayerOptions.obj" : ".\src\PlayerOptions.cpp" "$(INTDIR)"
	$(CPP) $(CPP_PROJ) ".\src\PlayerOptions.cpp"

"$(INTDIR)\Splash.obj" : ".\src\Splash.cpp" "$(INTDIR)"
	$(CPP) $(CPP_PROJ) ".\src\Splash.cpp"

"$(INTDIR)\Utils.obj" : ".\src\Utils.cpp" "$(INTDIR)"
	$(CPP) $(CPP_PROJ) ".\src\Utils.cpp"

"$(INTDIR)\Player.res" : ".\src\Player.rc" ".\src\PlayerResources.rc" ".\src\PlayerVersion.rc.in" ".\src\PlayerVersion.vc6.rc" "$(INTDIR)"
	$(RSC) $(RSC_PROJ) ".\src\Player.rc"

".\src\PlayerVersion.vc6.rc" : ".\src\PlayerVersion.rc.in" ".\VERSION"
	powershell -NoProfile -ExecutionPolicy Bypass -File ".\scripts\generate-player-version-rc.ps1" -Template ".\src\PlayerVersion.rc.in" -Output ".\src\PlayerVersion.vc6.rc" -VersionFile ".\VERSION"
