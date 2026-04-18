@echo off
powershell -NoProfile -ExecutionPolicy Bypass -File "%~dp0release.ps1" -Toolchain VC6 %*
