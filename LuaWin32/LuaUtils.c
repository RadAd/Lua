#include "LuaUtils.h"

#include <stdio.h>

const char* F_COUNT = "_len";

void lua_stackdump(lua_State* L)
{
    const int top = lua_gettop(L);
    int i;

    for (i = 1; i <= top; i++)
    {
        const int t = lua_type(L, i);
        switch (t) {
        case LUA_TSTRING:  /* strings */
            printf("`%s'", lua_tostring(L, i));
            break;
        case LUA_TBOOLEAN:  /* booleans */
            printf(lua_toboolean(L, i) ? "true" : "false");
            break;
        case LUA_TNUMBER:  /* numbers */
            printf("%g", lua_tonumber(L, i));
            break;
        default:  /* other values */
            printf("%s", lua_typename(L, t));
            break;
        }
        printf("  ");  /* put a separator */
    }
    printf("\n");  /* end the listing */
}
