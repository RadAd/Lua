@echo off
setlocal
prompt $G$S

rem call vcmake x86 release build x64 build

call :pack Win32 || exit /b 1
call :pack x64 || exit /b 1
goto :eof

:pack
if exist Lua_%1.zip del Lua_%1.zip
if exist LuaDev_%1.zip del LuaDev_%1.zip
rem zip -j Lua_%1.zip Bin\Release%1\Lua.exe Bin\Release%1\Lua.dll Bin\Release%1\lfs.dll Bin\Release%1\lrwin32.dll || exit /b 1
rem zip -j LuaDev_%1.zip Bin\Release%1\Lua.dll Bin\Release%1\Lua.lib lua\lauxlib.h lua\lua.h lua\luaconf.h lua\lualib.h || exit /b 1
7z a Lua_%1.zip Bin\Release%1\Lua.exe Bin\Release%1\Lua.dll Bin\Release%1\lfs.dll Bin\Release%1\lrwin32.dll register.bat > NUL || exit /b 1
7z rn Lua_%1.zip Bin\Release%1\Lua.exe Lua.exe Bin\Release%1\Lua.dll Lua.dll Bin\Release%1\lfs.dll Bin\Release%1\lrwin32.dll lfs.dll > NUL || exit /b 1
7z a LuaDev_%1.zip Bin\Release%1\Lua.dll Bin\Release%1\Lua.lib lua\lauxlib.h lua\lua.h lua\luaconf.h lua\lualib.h > NUL || exit /b
7z rn LuaDev_%1.zip Bin\Release%1\Lua.dll Bin%1\Lua.dll Bin\Release%1\Lua.lib Bin%1\Lua.lib lua\lauxlib.h include\lauxlib.h lua\lua.h include\lua.h lua\luaconf.h include\luaconf.h lua\lualib.h include\lualib.h > NUL || exit /b

goto :eof
