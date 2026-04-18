@echo off
powershell -NoProfile -ExecutionPolicy Bypass -File "%~dp0release.ps1" -Toolchain MSVC2022 %*
