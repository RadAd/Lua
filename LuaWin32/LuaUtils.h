#pragma once
#include <lua.h>
#include <lauxlib.h>
#include <assert.h>

void lua_stackdump(lua_State* L);

int l_newObject(lua_State* L);
int l_errorReadOnly(lua_State* L);

extern const char* F_COUNT;

inline void lua_newarray(lua_State* L)
{
    lua_newtable(L);
    lua_pushinteger(L, 0);
    lua_setfield(L, -2, F_COUNT);
}

inline void lua_appendarray(lua_State* L, int idx)
{
    lua_getfield(L, idx, F_COUNT);
    lua_Integer i = luaL_checkinteger(L, -1);
    lua_pop(L, 1);
    lua_pushinteger(L, ++i);
    lua_setfield(L, idx > 0 ? idx : idx - 1, F_COUNT);
    lua_rawseti(L, idx, i);
}

inline int lua_readOnly(lua_State* L, int index)
{
    // TODO Test
    const int top = lua_gettop(L);

    lua_newtable(L);
    lua_pushcfunction(L, l_newObject);
    lua_setfield(L, -2, "new");
    lua_pushvalue(L, -2);
    lua_setfield(L, -2, "__base");

    lua_newtable(L);
    lua_pushcfunction(L, l_errorReadOnly);
    lua_setfield(L, -2, "__newindex");
    lua_pushvalue(L, -3);
    lua_setfield(L, -2, "__index");

    lua_setmetatable(L, -2);

    // TODO Pop original table

    assert((top + 1) == lua_gettop(L));
    return 1;
}

typedef struct lua_RegInt {
    const char* name;
    lua_Integer value;
} lua_RegInt;

inline void rlua_newtableinteger(lua_State* L, const lua_RegInt* v, const char* name)
{
    const int top = lua_gettop(L);

    const int bReadOnly = 1;
    if (bReadOnly)
    {
        lua_newtable(L);
        lua_pushcfunction(L, l_newObject);
        lua_setfield(L, -2, "new");

        lua_newtable(L);
        lua_pushcfunction(L, l_errorReadOnly);
        lua_setfield(L, -2, "__newindex");
    }
    lua_newtable(L);
    for (; v->name != NULL; v++)
    {
        lua_pushinteger(L, v->value);
        lua_setfield(L, -2, v->name);
    }
    if (bReadOnly)
    {
        lua_pushvalue(L, -1);
        lua_setfield(L, -3, "__index");
        lua_setfield(L, -3, "__base");

        lua_setmetatable(L, -2);
    }
    lua_setfield(L, -2, name);

    assert(top == lua_gettop(L));
}
