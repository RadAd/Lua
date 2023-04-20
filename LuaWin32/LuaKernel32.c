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

static int l_ExpandEnvironmentStrings(lua_State* L)
{
    int arg = 0;
    const char* lpName = rlua_checkstring(L, ++arg);

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
        lua_pushstring(L, cb.str);
    return lua_gettop(L) - rt;
}

static int l_FindClose(lua_State* L)
{
    int arg = 0;
    HANDLE h = rlua_checkHANDLE(L, ++arg);

    BOOL ret = FindClose(h);

    const int rt = lua_gettop(L);
    rlua_pushBOOL(L, ret);
    return lua_gettop(L) - rt;
}

static int l_FindFirstFile(lua_State* L)
{
    int arg = 0;
    const char* lpFileName = rlua_checkstring(L, ++arg);
    const int FindFileDataidx = ++arg;

    WIN32_FIND_DATAA FindFileData;
    ZeroMemory(&FindFileData, sizeof(FindFileData));
    HANDLE h = FindFirstFile(lpFileName, &FindFileData);

    rlua_fromWIN32_FIND_DAT(L, FindFileDataidx, &FindFileData);

    const int rt = lua_gettop(L);
    rlua_pushHANDLE(L, h);
    return lua_gettop(L) - rt;
}

static int l_FindNextFile(lua_State* L)
{
    int arg = 0;
    HANDLE h = rlua_checkHANDLE(L, ++arg);
    const int FindFileDataidx = ++arg;

    WIN32_FIND_DATAA FindFileData;
    ZeroMemory(&FindFileData, sizeof(FindFileData));
    BOOL ret = FindNextFile(h, &FindFileData);

    rlua_fromWIN32_FIND_DAT(L, -1, &FindFileData);

    const int rt = lua_gettop(L);
    rlua_pushBOOL(L, ret);
    return lua_gettop(L) - rt;
}

static int l_GetEnvironmentStrings(lua_State* L)
{
    int arg = 0;

    const int rt = lua_gettop(L);

    lua_createtable(L, 0, 100);

    LPCH env = GetEnvironmentStrings();
    LPCCH e = env;
    while (*e != '\0')
    {
        const char* eq = strchr(e, L'=');
        if (eq != e)
        {
            lua_pushlstring(L, e, eq - e);
            lua_pushstring(L, eq + 1);
            lua_settable(L, -3);
        }

        e += strlen(e) + 1;
    }
    FreeEnvironmentStrings(env);

    return lua_gettop(L) - rt;
}

static int l_GetEnvironmentVariable(lua_State* L)
{
    int arg = 0;
    const char* lpName = rlua_checkstring(L, ++arg);

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
        lua_pushstring(L, cb.str);
    return lua_gettop(L) - rt;
}

static int l_GetCurrentDirectory(lua_State* L)
{
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
        lua_pushstring(L, cb.str);
    return lua_gettop(L) - rt;
}

static int l_GetLastError(lua_State* L)
{
    const DWORD e = GetLastError();

    const int rt = lua_gettop(L);
    rlua_pushDWORD(L, e);
    return lua_gettop(L) - rt;
}

static int l_OutputDebugString(lua_State* L)
{
    int arg = 0;
    const char* lpOutputString = rlua_checkstring(L, ++arg);

    OutputDebugString(lpOutputString);

    return 0;
}

static int l_SetEnvironmentVariable(lua_State* L)
{
    int arg = 0;
    const char* lpName = rlua_checkstring(L, ++arg);
    const char* lpValue = rlua_checkstringornil(L, ++arg);

    const BOOL ret = SetEnvironmentVariable(lpName, lpValue);

    const int rt = lua_gettop(L);
    rlua_pushBOOL(L, ret);
    return lua_gettop(L) - rt;
}

extern const struct luaL_Reg kernel32lib[] = {
  { "ExpandEnvironmentStrings", l_ExpandEnvironmentStrings },
  { "FindClose", l_FindClose },
  { "FindFirstFile", l_FindFirstFile },
  { "FindNextFile", l_FindNextFile },
  { "GetEnvironmentStrings", l_GetEnvironmentStrings },
  { "GetEnvironmentVariable", l_GetEnvironmentVariable },
  { "GetCurrentDirectory", l_GetCurrentDirectory},
  { "GetLastError", l_GetLastError },
  { "OutputDebugString", l_OutputDebugString },
  { "SetEnvironmentVariable", l_SetEnvironmentVariable },

  { NULL, 0 },
};
