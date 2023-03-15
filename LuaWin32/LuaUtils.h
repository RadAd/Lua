#pragma once
#include <lua.h>
#include <lauxlib.h>

void lua_stackdump(lua_State* L);

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

typedef struct lua_RegInt {
    const char* name;
    int value;
} lua_RegInt;

inline void rlua_newtableinteger(lua_State* L, const lua_RegInt* v, const char* name)
{
    lua_newtable(L);
    for (; v->name != NULL; v++)
    {
        lua_pushinteger(L, (lua_Integer) v->value);
        lua_setfield(L, -2, v->name);
    }
    lua_setfield(L, -2, name);
}
