@echo off
setlocal ENABLEDELAYEDEXPANSION
set LUA="%~dp0lua.exe"
if not exist %LUA% echo Lua.exe not found && exit /b 1

reg add "HKEY_CURRENT_USER\SOFTWARE\Microsoft\Windows\CurrentVersion\App Paths\Lua.exe" /ve /f /d %LUA%

set CMD=Lua.exe "%%1" %%*

rem echo --- %CMD%
set CMD=!CMD:^%%=^%%^%%!
set CMD=!CMD:"="""!
rem echo --- %CMD%

call :register .lua RadLua "%CMD%"
goto :eof

:register
if not "%~4" == "" (echo Too manu arguments && exit /b 1)
reg add "HKEY_CURRENT_USER\SOFTWARE\Classes\%~1" /ve /f /d %2
reg add "HKEY_CURRENT_USER\SOFTWARE\Classes\%~2\shell\open\command" /ve /f /d %3
goto :eof
