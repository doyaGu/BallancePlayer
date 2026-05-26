# Convenience nmake entry point for the VC6 DSP projects.
#
# Examples:
#   nmake /f Makefile
#   nmake /f Makefile CFG=Release
#   nmake /f Makefile CFG=Debug
#   nmake /f Makefile clean

!IF "$(VC6_ROOT)" != ""
NMAKE="$(VC6_ROOT)\Bin\nmake.exe"
!ELSE
NMAKE=nmake.exe
!ENDIF

!IF "$(CFG)" == ""
CFG=Debug
!ENDIF

!IF "$(CFG)" != "Debug" && "$(CFG)" != "Release"
!ERROR Invalid CFG "$(CFG)". Use "Debug" or "Release".
!ENDIF

!IF "$(TARGET)" == ""
TARGET=All
!ENDIF

!IF "$(TARGET)" == "Player"
build : player
!ELSEIF "$(TARGET)" == "ConfigTool"
build : configtool
!ELSEIF "$(TARGET)" == "All"
build : player configtool
!ELSE
!ERROR Invalid TARGET "$(TARGET)". Use "Player", "ConfigTool", or "All".
!ENDIF

player :
	$(NMAKE) /nologo /f Player.mak CFG="Player - Win32 $(CFG)"

configtool :
	$(NMAKE) /nologo /f ConfigTool.mak CFG="ConfigTool - Win32 $(CFG)"

clean :
	$(NMAKE) /nologo /f Player.mak CFG="Player - Win32 $(CFG)" CLEAN
	$(NMAKE) /nologo /f ConfigTool.mak CFG="ConfigTool - Win32 $(CFG)" CLEAN
