#include <lua.h>
#include <Windows.h>

#include "Buffer.h"
#include "LuaUtils.h"
#include "LuaWin32Types.h"

static int l_GetEnvironmentVariable(lua_State* L)
{
    int arg = 0;
    const char* lpName = rlua_checkstring(L, ++arg);

    static CharBuffer cb;
    if (cb.size == 0)
        cb = CharBufferCreate();

    DWORD len;
    while ((len = GetEnvironmentVariableA(lpName, cb.str, cb.size)) >= cb.size)
        CharBufferIncreaseSize(L, &cb, len + 1);

    if (len == 0)
    {
        const DWORD e = GetLastError();
        if (e == ERROR_ENVVAR_NOT_FOUND)
        {
            const int rt = lua_gettop(L);
            lua_pushnil(L);
            return lua_gettop(L) - rt;
        }
        else
            return luaL_error(L, "GetEnvironmentVariable failed %d\n", e);
    }
    else
    {
        const int rt = lua_gettop(L);
        lua_pushstring(L, cb.str);
        return lua_gettop(L) - rt;
    }
}

static int l_GetLastError(lua_State* L)
{
    DWORD e = GetLastError();

    const int rt = lua_gettop(L);
    rlua_pushDWORD(L, e);
    return lua_gettop(L) - rt;
}

static int l_OutputDebugString(lua_State* L)
{
    int arg = 0;
    const char* lpOutputString = rlua_checkstring(L, ++arg);

    OutputDebugStringA(lpOutputString);

    return 0;
}

extern const struct luaL_Reg kernel32lib[] = {
  { "GetEnvironmentVariable", l_GetEnvironmentVariable },
  { "GetLastError", l_GetLastError },
  { "OutputDebugString", l_OutputDebugString },

  { NULL, 0 },
};
