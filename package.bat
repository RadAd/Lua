setlocal
prompt $G$S

rem call vcmake x86 release build x64 build

call :pack Win32 || exit /b 1
call :pack x64 || exit /b 1
goto :eof

:pack
del Lua_%1.zip
del LuaDev_%1.zip
zip -j Lua_%1.zip Bin\Release%1\Lua.exe Bin\Release%1\Lua.dll Bin\Release%1\lfs.dll || exit /b 1
zip -j LuaDev_%1.zip Bin\Release%1\Lua.dll Bin\Release%1\Lua.lib lua\lauxlib.h lua\lua.h lua\luaconf.h lua\lualib.h || exit /b 1
