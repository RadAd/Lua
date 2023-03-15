#include "lua.h"

#include <Windows.h>

#include "LuaUtils.h"
#include "LuaWin32Types.h"

static int l_GetLastError(lua_State* L)
{
    DWORD e = GetLastError();

    const int rt = lua_gettop(L);
    rlua_pushDWORD(L, e);
    return lua_gettop(L) - rt;
}

extern const struct luaL_Reg kernel32lib[] = {
  { "GetLastError", l_GetLastError },

  { NULL, 0 },
};
