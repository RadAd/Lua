#include <lua.h>
#include <Windows.h>
//#include <string.h>

static char* strndup(const char* str, int chars)
{
    char* buffer = (char*) malloc(chars + 1);
    if (buffer)
    {
        strncpy_s(buffer, chars + 1, str, chars);
        buffer[chars] = 0;
    }

    return buffer;
}

#include "Buffer.h"
#include "LuaUtils.h"
#include "LuaWin32Types.h"

static int l_CreateFile(lua_State* L)
{
    int arg = 0;
    const char* const lpFileName = rlua_checkstring(L, ++arg);
    const DWORD dwDesiredAccess = rlua_checkDWORD(L, ++arg);
    const DWORD dwShareMode = rlua_checkDWORD(L, ++arg);
    SECURITY_ATTRIBUTES* lpSecurityAttributes = rlua_optSECURITY_ATTRIBUTES(L, ++arg);
    const DWORD dwCreationDisposition = rlua_checkDWORD(L, ++arg);
    const DWORD dwFlagsAndAttributes = rlua_optDWORD(L, ++arg, FILE_ATTRIBUTE_NORMAL);
    const HANDLE hTemplateFile = rlua_optHANDLE(L, ++arg, NULL);
    luaL_checktype(L, arg + 1, LUA_TNONE);

    if (lpSecurityAttributes)
        lpSecurityAttributes->nLength = sizeof(SECURITY_ATTRIBUTES);

    const HANDLE h = CreateFile(lpFileName, dwDesiredAccess, dwShareMode, lpSecurityAttributes, dwCreationDisposition, dwFlagsAndAttributes, hTemplateFile);

    free(lpSecurityAttributes);

    const int rt = lua_gettop(L);
    rlua_pushHANDLE(L, h);
    return lua_gettop(L) - rt;
}

static int l_CloseHandle(lua_State* L)
{
    int arg = 0;
    const HANDLE h = rlua_checkHANDLE(L, ++arg);
    luaL_checktype(L, arg + 1, LUA_TNONE);

    const BOOL ret = CloseHandle(h);

    const int rt = lua_gettop(L);
    rlua_pushBOOL(L, ret);
    return lua_gettop(L) - rt;
}

static int l_ExpandEnvironmentStrings(lua_State* L)
{
    int arg = 0;
    const char* const lpName = rlua_checkstring(L, ++arg);
    luaL_checktype(L, arg + 1, LUA_TNONE);

    static CharBuffer cb;
    if (cb.size == 0)
        cb = CharBufferCreate();

    DWORD len;
    while ((len = ExpandEnvironmentStrings(lpName, cb.str, cb.size)) >= cb.size)
        CharBufferIncreaseSize(L, &cb, len + 1);

    const int rt = lua_gettop(L);
    if (len == 0)
        lua_pushnil(L);
    else
        lua_pushlstring(L, cb.str, len);
    return lua_gettop(L) - rt;
}

static int l_FindClose(lua_State* L)
{
    int arg = 0;
    const HANDLE h = rlua_checkHANDLE(L, ++arg);
    luaL_checktype(L, arg + 1, LUA_TNONE);

    const BOOL ret = FindClose(h);

    const int rt = lua_gettop(L);
    rlua_pushBOOL(L, ret);
    return lua_gettop(L) - rt;
}

static int l_FindFirstFile(lua_State* L)
{
    int arg = 0;
    const char* const lpFileName = rlua_checkstring(L, ++arg);
    const int FindFileDataidx = ++arg;
    luaL_checktype(L, arg + 1, LUA_TNONE);

    WIN32_FIND_DATAA FindFileData;
    ZeroMemory(&FindFileData, sizeof(FindFileData));
    const HANDLE h = FindFirstFile(lpFileName, &FindFileData);

    rlua_fromWIN32_FIND_DATA(L, FindFileDataidx, &FindFileData);

    const int rt = lua_gettop(L);
    rlua_pushHANDLE(L, h);
    return lua_gettop(L) - rt;
}

static int l_FindNextFile(lua_State* L)
{
    int arg = 0;
    const HANDLE h = rlua_checkHANDLE(L, ++arg);
    const int FindFileDataidx = ++arg;
    luaL_checktype(L, arg + 1, LUA_TNONE);

    WIN32_FIND_DATAA FindFileData;
    ZeroMemory(&FindFileData, sizeof(FindFileData));
    const BOOL ret = FindNextFile(h, &FindFileData);

    rlua_fromWIN32_FIND_DATA(L, -1, &FindFileData);

    const int rt = lua_gettop(L);
    rlua_pushBOOL(L, ret);
    return lua_gettop(L) - rt;
}

static int l_GetConsoleAliases(lua_State* L)
{
    int arg = 0;
    const char* const lpExeName = rlua_checkstring(L, ++arg);
    luaL_checktype(L, arg + 1, LUA_TNONE);

    const int rt = lua_gettop(L);
    const DWORD size = GetConsoleAliasesLength((char*) lpExeName) / sizeof(TCHAR);

    static CharBuffer cb;
    if (cb.size == 0)
        cb = CharBufferCreateSize(size);
    else
        CharBufferIncreaseSize(L, &cb, size);

    if (!GetConsoleAliases(cb.str, cb.size, (char*) lpExeName))
        lua_pushnil(L);
    else
    {
        lua_createtable(L, 0, 100);
        for (const char* a = cb.str; (DWORD)(a - cb.str) < cb.size; a += strlen(a) + 1)
        {
            const char* eq = strchr(a, L'=');
            if (eq != a)
            {
                lua_pushlstring(L, a, eq - a);
                lua_pushstring(L, eq + 1);
                lua_settable(L, -3);
            }
        }
    }

    return lua_gettop(L) - rt;
}

static int l_GetConsoleAliasesLength(lua_State* L)
{
    int arg = 0;
    const char* const lpExeName = rlua_checkstring(L, ++arg);
    luaL_checktype(L, arg + 1, LUA_TNONE);

    const int rt = lua_gettop(L);

    lua_pushinteger(L, GetConsoleAliasesLength((char*) lpExeName));

    return lua_gettop(L) - rt;
}

static int l_GetCurrentDirectory(lua_State* L)
{
    int arg = 0;
    luaL_checktype(L, arg + 1, LUA_TNONE);

    static CharBuffer cb;
    if (cb.size == 0)
        cb = CharBufferCreate();

    DWORD len;
    while ((len = GetCurrentDirectory(cb.size, cb.str)) >= cb.size)
        CharBufferIncreaseSize(L, &cb, len + 1);

    const int rt = lua_gettop(L);
    if (len == 0)
        lua_pushnil(L);
    else
        lua_pushlstring(L, cb.str, len);
    return lua_gettop(L) - rt;
}

static int l_GetCurrentProcess(lua_State* L)
{
    int arg = 0;
    luaL_checktype(L, arg + 1, LUA_TNONE);

    const HANDLE h = GetCurrentProcess();

    const int rt = lua_gettop(L);
    rlua_pushHANDLE(L, h);
    return lua_gettop(L) - rt;
}

static int l_GetEnvironmentStrings(lua_State* L)
{
    int arg = 0;
    luaL_checktype(L, arg + 1, LUA_TNONE);

    const int rt = lua_gettop(L);

    lua_createtable(L, 0, 100);

    LPCH env = GetEnvironmentStrings();
    for (LPCCH e = env; *e != '\0'; e += strlen(e) + 1)
    {
        const char* eq = strchr(e, L'=');
        if (eq != e)
        {
            lua_pushlstring(L, e, eq - e);
            lua_pushstring(L, eq + 1);
            lua_settable(L, -3);
        }
    }
    FreeEnvironmentStrings(env);

    return lua_gettop(L) - rt;
}

static int l_GetEnvironmentVariable(lua_State* L)
{
    int arg = 0;
    const char* const lpName = rlua_checkstring(L, ++arg);
    luaL_checktype(L, arg + 1, LUA_TNONE);

    static CharBuffer cb;
    if (cb.size == 0)
        cb = CharBufferCreate();

    DWORD len;
    while ((len = GetEnvironmentVariable(lpName, cb.str, cb.size)) >= cb.size)
        CharBufferIncreaseSize(L, &cb, len + 1);

    const int rt = lua_gettop(L);
    if (len == 0)
        lua_pushnil(L);
    else
        lua_pushlstring(L, cb.str, len);
    return lua_gettop(L) - rt;
}

static int l_GetLastError(lua_State* L)
{
    int arg = 0;
    luaL_checktype(L, arg + 1, LUA_TNONE);

    const DWORD e = GetLastError();

    const int rt = lua_gettop(L);
    rlua_pushDWORD(L, e);
    return lua_gettop(L) - rt;
}

static int l_GetProcessId(lua_State* L)
{
    int arg = 0;
    const HANDLE h = rlua_checkHANDLE(L, ++arg);
    luaL_checktype(L, arg + 1, LUA_TNONE);

    const DWORD v = GetProcessId(h);

    const int rt = lua_gettop(L);
    rlua_pushDWORD(L, v);
    return lua_gettop(L) - rt;
}

static int l_GetStdHandle(lua_State* L)
{
    int arg = 0;
    const DWORD n = rlua_checkDWORD(L, ++arg);
    luaL_checktype(L, arg + 1, LUA_TNONE);

    const HANDLE h = GetStdHandle(n);

    const int rt = lua_gettop(L);
    rlua_pushHANDLE(L, h);
    return lua_gettop(L) - rt;
}

static int l_GetTickCount(lua_State* L)
{
    int arg = 0;
    luaL_checktype(L, arg + 1, LUA_TNONE);

    const DWORD v = GetTickCount();

    const int rt = lua_gettop(L);
    rlua_pushDWORD(L, v);
    return lua_gettop(L) - rt;
}

static int l_OutputDebugString(lua_State* L)
{
    int arg = 0;
    const char* const lpOutputString = rlua_checkstring(L, ++arg);
    luaL_checktype(L, arg + 1, LUA_TNONE);

    OutputDebugString(lpOutputString);

    return 0;
}
    
static int l_ReadConsole(lua_State* L)
{
    int arg = 0;
    const HANDLE hFile = rlua_checkHANDLE(L, ++arg);
    const DWORD nNumberOfCharsToRead = rlua_checkDWORD(L, ++arg);
    const char* const lpInputString = rlua_optstring(L, ++arg, NULL);
    DWORD NumberOfCharsRead = 0;
    const PCONSOLE_READCONSOLE_CONTROL pInputControl = rlua_optCONSOLE_READCONSOLE_CONTROL(L, ++arg);
    luaL_checktype(L, arg + 1, LUA_TNONE);

    static CharBuffer cb;
    if (cb.size == 0)
        cb = CharBufferCreate();
    CharBufferIncreaseSize(L, &cb, nNumberOfCharsToRead);

    if (pInputControl)
        pInputControl->nLength = sizeof(CONSOLE_READCONSOLE_CONTROL);

    BOOL ret = FALSE;
    if (pInputControl && (pInputControl->nInitialChars == 0 || lpInputString))
    {
        WCHAR buffer[1024];
        MultiByteToWideChar(CP_UTF8, 0, lpInputString, pInputControl->nInitialChars, buffer, ARRAYSIZE(buffer));
        ret = ReadConsoleW(hFile, buffer, nNumberOfCharsToRead, &NumberOfCharsRead, pInputControl);
        int bytes = WideCharToMultiByte(CP_UTF8, 0, buffer, NumberOfCharsRead, cb.str, cb.size, NULL, NULL);
    }
    else
    {
        ret = ReadConsole(hFile, cb.str, nNumberOfCharsToRead, &NumberOfCharsRead, pInputControl);
    }

    const int rt = lua_gettop(L);
    rlua_pushBOOL(L, ret);
    if (ret)
        lua_pushlstring(L, cb.str, NumberOfCharsRead);
    else
        lua_pushnil(L);
    rlua_pushDWORD(L, NumberOfCharsRead);
    return lua_gettop(L) - rt;
}

static int l_ReadFile(lua_State* L)
{
    int arg = 0;
    const HANDLE hFile = rlua_checkHANDLE(L, ++arg);
    //const char* lpBuffer = NULL;
    const DWORD nNumberOfBytesToRead = rlua_checkDWORD(L, ++arg);
    DWORD NumberOfBytesRead = 0;
    const LPOVERLAPPED lpOverlapped = NULL;
    luaL_checktype(L, arg + 1, LUA_TNONE);

    static CharBuffer cb;
    if (cb.size == 0)
        cb = CharBufferCreate();
    CharBufferIncreaseSize(L, &cb, nNumberOfBytesToRead);

    const BOOL ret = ReadFile(hFile, cb.str, nNumberOfBytesToRead, &NumberOfBytesRead, lpOverlapped);

    const int rt = lua_gettop(L);
    rlua_pushBOOL(L, ret);
    if (ret)
        lua_pushlstring(L, cb.str, NumberOfBytesRead);
    else
        lua_pushnil(L);
    rlua_pushDWORD(L, NumberOfBytesRead);
    return lua_gettop(L) - rt;
}

static int l_SetEnvironmentVariable(lua_State* L)
{
    int arg = 0;
    const char* const lpName = rlua_checkstring(L, ++arg);
    const char* const lpValue = rlua_optstring(L, ++arg, NULL);
    luaL_checktype(L, arg + 1, LUA_TNONE);

    const BOOL ret = SetEnvironmentVariable(lpName, lpValue);

    const int rt = lua_gettop(L);
    rlua_pushBOOL(L, ret);
    return lua_gettop(L) - rt;
}

static int l_Sleep(lua_State* L)
{
    int arg = 0;
    const DWORD dwMilliSeconds = rlua_checkDWORD(L, ++arg);
    luaL_checktype(L, arg + 1, LUA_TNONE);

    Sleep(dwMilliSeconds);

    const int rt = lua_gettop(L);
    return lua_gettop(L) - rt;
}

static int l_WriteConsole(lua_State* L)
{
    int arg = 0;
    const HANDLE hFile = rlua_checkHANDLE(L, ++arg);
    const char* lpBuffer = rlua_checkstring(L, ++arg);
    const DWORD nNumberOfCharsToWrite = rlua_checkDWORD(L, ++arg);
    DWORD NumberOfCharsWritten = 0;
    luaL_checktype(L, arg + 1, LUA_TNONE);

    const BOOL ret = WriteConsole(hFile, lpBuffer, nNumberOfCharsToWrite, &NumberOfCharsWritten, NULL);

    const int rt = lua_gettop(L);
    rlua_pushBOOL(L, ret);
    rlua_pushDWORD(L, NumberOfCharsWritten);
    return lua_gettop(L) - rt;
}

static int l_WriteFile(lua_State* L)
{
    int arg = 0;
    const HANDLE hFile = rlua_checkHANDLE(L, ++arg);
    const char* lpBuffer = rlua_checkstring(L, ++arg);
    const DWORD nNumberOfBytesToWrite = rlua_checkDWORD(L, ++arg);
    DWORD NumberOfBytesWritten = 0;
    const LPOVERLAPPED lpOverlapped = NULL;
    luaL_checktype(L, arg + 1, LUA_TNONE);

    const BOOL ret = WriteFile(hFile, lpBuffer, nNumberOfBytesToWrite, &NumberOfBytesWritten, lpOverlapped);

    const int rt = lua_gettop(L);
    rlua_pushBOOL(L, ret);
    rlua_pushDWORD(L, NumberOfBytesWritten);
    return lua_gettop(L) - rt;
}

extern const struct luaL_Reg kernel32lib[] = {
  { "CreateFile", l_CreateFile },
  { "CloseHandle", l_CloseHandle },
  { "ExpandEnvironmentStrings", l_ExpandEnvironmentStrings },
  { "FindClose", l_FindClose },
  { "FindFirstFile", l_FindFirstFile },
  { "FindNextFile", l_FindNextFile },
  { "GetConsoleAliases", l_GetConsoleAliases },
  { "GetConsoleAliasesLength", l_GetConsoleAliasesLength },
  { "GetCurrentDirectory", l_GetCurrentDirectory},
  { "GetCurrentProcess", l_GetCurrentProcess },
  { "GetEnvironmentStrings", l_GetEnvironmentStrings },
  { "GetEnvironmentVariable", l_GetEnvironmentVariable },
  { "GetLastError", l_GetLastError },
  { "GetProcessId", l_GetProcessId },
  { "GetStdHandle", l_GetStdHandle },
  { "GetTickCount", l_GetTickCount },
  { "OutputDebugString", l_OutputDebugString },
  { "ReadConsole", l_ReadConsole },
  { "ReadFile", l_ReadFile },
  { "SetEnvironmentVariable", l_SetEnvironmentVariable },
  { "Sleep", l_Sleep },
  { "WriteConsole", l_WriteConsole },
  { "WriteFile", l_WriteFile },

  { NULL, 0 },
};
