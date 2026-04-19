@echo off
setlocal

wscript.exe "%~dp0ConfigTool.vbs" %*
exit /b %ERRORLEVEL%
