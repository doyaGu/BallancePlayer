# Convenience nmake entry point for the VC6 DSP projects.
#
# Examples:
#   nmake /f Makefile
#   nmake /f Makefile CFG=Release
#   nmake /f Makefile TARGET=Player CFG=Debug
#   nmake /f Makefile clean

VC6_ROOT=C:\Users\kakut\SDK\VC6
NMAKE="$(VC6_ROOT)\Bin\nmake.exe"

!IF "$(CFG)" == ""
CFG=Debug
!ENDIF

!IF "$(CFG)" != "Debug" && "$(CFG)" != "Release"
!ERROR Invalid CFG "$(CFG)". Use "Debug" or "Release".
!ENDIF

!IF "$(TARGET)" == ""
TARGET=all
!ENDIF

!IF "$(TARGET)" == "ConfigTool"
build : configtool
!ELSEIF "$(TARGET)" == "Player"
build : player
!ELSE
build : all
!ENDIF

all : configtool player

configtool :
	$(NMAKE) /nologo /f ConfigTool.mak CFG="ConfigTool - Win32 $(CFG)"

player :
	$(NMAKE) /nologo /f Player.mak CFG="Player - Win32 $(CFG)"

clean :
	$(NMAKE) /nologo /f ConfigTool.mak CFG="ConfigTool - Win32 $(CFG)" CLEAN
	$(NMAKE) /nologo /f Player.mak CFG="Player - Win32 $(CFG)" CLEAN
