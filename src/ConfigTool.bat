@echo off
setlocal

pushd "%~dp0" >nul || exit /b 1
start "" /wait "%~dp0Player.exe" --config-tool %*
set "_configtool_exit=%ERRORLEVEL%"
popd >nul

exit /b %_configtool_exit%
