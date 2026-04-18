@echo off
powershell -NoProfile -ExecutionPolicy Bypass -File "%~dp0package.ps1" -Toolchain MSVC2022 %*
