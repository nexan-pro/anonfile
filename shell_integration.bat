@echo off

if not "%1"=="am_admin" (powershell start -verb runas '%0' am_admin & exit /b)
SET mypath=%~dp0anonfile_win32.exe
REG ADD "HKEY_CLASSES_ROOT\*\shell\anonfile" /d "-> Upload file to anonfile.com <-"
REG ADD "HKEY_CLASSES_ROOT\*\shell\anonfile" /v ICON /t REG_SZ /d %mypath%
REG ADD "HKEY_CLASSES_ROOT\*\shell\anonfile\command" /t REG_SZ /d "%mypath% \"%%1"\""