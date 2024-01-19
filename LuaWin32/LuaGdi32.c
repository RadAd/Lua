#include "lua.h"

#include <Windows.h>

#include "LuaUtils.h"
#include "LuaWin32Types.h"

static int l_TextOut(lua_State* L)
{
    int arg = 0;
    const HDC hdc = rlua_checkHDC(L, ++arg);
    const int x = rlua_checkint(L, ++arg);
    const int y = rlua_checkint(L, ++arg);
    const char* const str = luaL_checkstring(L, ++arg);
    const int c = rlua_optint(L, ++arg, (int) strlen(str));

    BOOL r = TextOut(hdc, x, y, str, c);

    const int rt = lua_gettop(L);
    rlua_pushBOOL(L, r);
    return lua_gettop(L) - rt;
}

extern const struct luaL_Reg gdi32lib[] = {
  { "TextOut", l_TextOut },

  { NULL, 0 },
};
