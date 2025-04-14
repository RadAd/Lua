@echo off
setlocal
prompt $G$S

rem call vcmake x86 release build x64 build

if [%1]==[] (
	call :pack Win32 || exit /b 1
	call :pack x64 || exit /b 1
) else if [%1]==[x86] (
	call :pack Win32 || exit /b 1
) else (
	call :pack %1 || exit /b 1
)
goto :eof

:pack
echo Pack %1
if exist Lua_%1.zip del Lua_%1.zip
if exist LuaDev_%1.zip del LuaDev_%1.zip
rem zip -j Lua_%1.zip Bin\Release%1\Lua.exe Bin\Release%1\Lua54.dll || exit /b 1
rem zip -j LuaDev_%1.zip Bin\Release%1\Lua54.dll Bin\Release%1\Lua54.lib lua\lauxlib.h lua\lua.h lua\luaconf.h lua\lualib.h || exit /b 1
7z a Lua_%1.zip Bin\Release%1\Lua.exe Bin\Release%1\Lua54.dll register.bat > NUL || exit /b 1
7z rn Lua_%1.zip Bin\Release%1\Lua.exe Lua.exe Bin\Release%1\Lua54.dll Lua54.dll > NUL || exit /b 1
7z a LuaDev_%1.zip Bin\Release%1\Lua54.dll Bin\Release%1\Lua54.lib lua\lauxlib.h lua\lua.h lua\luaconf.h lua\lualib.h > NUL || exit /b
7z rn LuaDev_%1.zip Bin\Release%1\Lua54.dll Bin%1\Lua54.dll Bin\Release%1\Lua54.lib Bin%1\Lua54.lib lua\lauxlib.h include\lauxlib.h lua\lua.h include\lua.h lua\luaconf.h include\luaconf.h lua\lualib.h include\lualib.h > NUL || exit /b

goto :eof
