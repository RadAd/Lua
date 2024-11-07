#include "LuaUtils.h"

#include <stdio.h>
#include <assert.h>

const char* F_COUNT = "_len";

void print_value(lua_State* L, int index)
{
    const int t = lua_type(L, index);
    switch (t)
    {
    case LUA_TSTRING:  /* strings */
        printf("`%s'", lua_tostring(L, index));
        break;
    case LUA_TBOOLEAN:  /* booleans */
        printf(lua_toboolean(L, index) ? "true" : "false");
        break;
    case LUA_TNUMBER:  /* numbers */
        printf("%g", lua_tonumber(L, index));
        break;
    default:  /* other values */
        printf("%s", lua_typename(L, t));
        break;
    }
}

void print_table(lua_State* L, int index)
{
    lua_pushnil(L);  /* first key */
    const int top = lua_gettop(L);
    while (lua_next(L, index < 0 ? index - 1 : index) != 0) {
        //lua_stackdump(L);
        /* uses 'key' (at index -2) and 'value' (at index -1) */
        print_value(L, -2);
        printf(" - ");
        print_value(L, -1);
        printf("\n");
        /* removes 'value'; keeps 'key' for next iteration */
        lua_pop(L, 1);
        assert(top == lua_gettop(L));
    }
}

void lua_stackdump(lua_State* L)
{
    const int top = lua_gettop(L);
    for (int i = 1; i <= top; i++)
    {
        print_value(L, i);
        printf("  ");  /* put a separator */
    }
    printf("\n");  /* end the listing */
}

void shallow_copy(lua_State* L, int index) {

    /*Create a new table on the stack.*/
    lua_newtable(L);

    lua_pushnil(L);
    while (lua_next(L, index < 0 ? index - 2 : index) != 0)
    {
        /*Need to duplicate the key, as we need to set it
        (one pop) and keep it for lua_next (the next pop). Stack looks like table, k, v.*/
        lua_pushvalue(L, -2);

        /*Now the stack looks like table, k, v, k.
        But now the key is on top. Settable expects the value to be on top. So we
        need to do a swaparooney.*/
        lua_insert(L, -2);

        /*Now we just set them. Stack looks like table,k,k,v, so the table is at -4*/
        lua_settable(L, -4);

        /*Now the key and value were set in the table, and we popped off, so we have
        table, k on the stack- which is just what lua_next wants, as it wants to find
        the next key on top. So we're good.*/
    }
}


int l_newObject(lua_State* L)
{
    const int rt = lua_gettop(L);
    lua_stackdump(L);
    lua_getfield(L, -1, "__base");
    shallow_copy(L, -1);
    lua_stackdump(L);
    return lua_gettop(L) - rt;
}

int l_errorReadOnly(lua_State* L)
{
    lua_pushliteral(L, "attempt to update a read-only table");
    lua_error(L);
    return 0;
}
