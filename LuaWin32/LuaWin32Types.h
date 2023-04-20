#pragma once

#define REGISTER_CHECK_INTEGER(type) \
inline type rlua_check##type(lua_State* L, int idx) \
{ \
    return (type) luaL_checkinteger(L, idx); \
} \
inline type rlua_opt##type(lua_State* L, int idx, type def) \
{ \
    return (type) luaL_optinteger(L, idx, (lua_Integer) def); \
} \
inline type rlua_getfield##type(lua_State* L, int idx, const char* k) \
{ \
    lua_getfield(L, idx, k); \
    type i = rlua_check##type(L, -1); \
    lua_pop(L, 1); \
    return i; \
} \
inline type rlua_getfieldopt##type(lua_State* L, int idx, const char* k, type def) \
{ \
    lua_getfield(L, idx, k); \
    type i = rlua_opt##type(L, -1, def); \
    lua_pop(L, 1); \
    return i; \
} \
inline void rlua_push##type(lua_State* L, type v) \
{ \
    lua_pushinteger(L, (lua_Integer) v); \
} \
inline void rlua_setfield##type(lua_State* L, int idx, const char* k, type v) \
{ \
    rlua_push##type(L, v); \
    lua_setfield(L, idx > 0 ? idx : idx - 1, k); \
}

REGISTER_CHECK_INTEGER(int)
REGISTER_CHECK_INTEGER(UINT)
REGISTER_CHECK_INTEGER(LONG)
REGISTER_CHECK_INTEGER(LONG_PTR)
REGISTER_CHECK_INTEGER(DWORD)
REGISTER_CHECK_INTEGER(WPARAM)
REGISTER_CHECK_INTEGER(LPARAM)
REGISTER_CHECK_INTEGER(LRESULT)
REGISTER_CHECK_INTEGER(ATOM)
REGISTER_CHECK_INTEGER(HANDLE)
REGISTER_CHECK_INTEGER(HWND)
REGISTER_CHECK_INTEGER(HDC)
REGISTER_CHECK_INTEGER(HICON)
REGISTER_CHECK_INTEGER(HBRUSH)
REGISTER_CHECK_INTEGER(HCURSOR)

// string

inline const char* rlua_checkstring(lua_State* L, int idx)
{
    return luaL_checkstring(L, idx);
}
inline const char* rlua_checkstringornil(lua_State* L, int idx)
{
    return lua_isnil(L, idx) ? NULL : luaL_checkstring(L, idx);
}
inline const char* rlua_optstring(lua_State* L, int idx, const char* def)
{
    return luaL_optstring(L, idx, def);
}
inline const char* rlua_getfieldstring(lua_State* L, int idx, const char* k)
{
    lua_getfield(L, idx, k);
    const char* i = rlua_checkstring(L, -1);
    lua_pop(L, 1);
    return i;
}
inline const char* rlua_getfieldoptstring(lua_State* L, int idx, const char* k, const char* def)
{
    lua_getfield(L, idx, k);
    const char* i = rlua_optstring(L, -1, def);
    lua_pop(L, 1);
    return i;
}
inline void rlua_pushstring(lua_State* L, const char* v)
{
    lua_pushstring(L, v);
}
inline void rlua_setfieldstring(lua_State* L, int idx, const char* k, const char* v)
{
    rlua_pushstring(L, v);
    lua_setfield(L, idx > 0 ? idx : idx - 1, k);
}

// BOOL

inline BOOL rlua_checkBOOL(lua_State* L, int idx)
{
    luaL_checktype(L, idx, LUA_TBOOLEAN);
    return (BOOL) lua_toboolean(L, idx);
}
inline BOOL rlua_optBOOL(lua_State* L, int idx, BOOL def)
{
    return (BOOL) luaL_opt(L, rlua_checkBOOL, idx, def);
}
inline BOOL rlua_getfieldBOOL(lua_State* L, int idx, const char* k)
{
    lua_getfield(L, idx, k);
    BOOL i = rlua_checkBOOL(L, -1);
    lua_pop(L, 1);
    return i;
}
inline BOOL rlua_getfieldoptBOOL(lua_State* L, int idx, const char* k, BOOL def)
{
    lua_getfield(L, idx, k);
    BOOL i = rlua_optBOOL(L, -1, def);
    lua_pop(L, 1);
    return i;
}
inline void rlua_pushBOOL(lua_State* L, BOOL v)
{
    lua_pushboolean(L, v);
}
inline void rlua_setfieldBOOL(lua_State* L, int idx, const char* k, BOOL v)
{
    rlua_pushBOOL(L, v);
    lua_setfield(L, idx > 0 ? idx : idx - 1, k);
}

// Structs -------------------------------

#define REGISTER_CHECK_STRUCT(type) \
inline type rlua_check##type(lua_State* L, int idx) \
{ \
    luaL_checktype(L, idx, LUA_TTABLE); \
    type v; \
    ZeroMemory(&v, sizeof(v)); \
    rlua_to##type(L, -1, &v); \
    return v; \
} \
inline void rlua_push##type(lua_State* L, const type v) \
{ \
    lua_newtable(L); \
    rlua_from##type(L, -1, v); \
} \
inline type rlua_getfield##type(lua_State* L, int idx, const char* k) \
{ \
    lua_getfield(L, idx, k); \
    type v = rlua_check##type(L, -1); \
    lua_pop(L, 1); \
    return v; \
} \
inline void rlua_setfield##type(lua_State* L, int idx, const char* k, const type v) \
{ \
    rlua_push##type(L, v); \
    lua_setfield(L, idx > 0 ? idx : idx - 1, k); \
}

// RECT

inline void rlua_fromRECT(lua_State* L, int idx, const RECT rc)
{
    luaL_checktype(L, idx, LUA_TTABLE);
    rlua_setfieldLONG(L, idx, "left", rc.left);
    rlua_setfieldLONG(L, idx, "top", rc.top);
    rlua_setfieldLONG(L, idx, "right", rc.right);
    rlua_setfieldLONG(L, idx, "bottom", rc.bottom);
}

inline void rlua_toRECT(lua_State* L, int idx, RECT* prc)
{
    luaL_checktype(L, idx, LUA_TTABLE);
    prc->left = rlua_getfieldLONG(L, idx, "left");
    prc->top = rlua_getfieldLONG(L, idx, "top");
    prc->right = rlua_getfieldLONG(L, idx, "right");
    prc->bottom = rlua_getfieldLONG(L, idx, "bottom");
}

REGISTER_CHECK_STRUCT(RECT)

// POINT

inline void rlua_fromPOINT(lua_State* L, int idx, const POINT pt)
{
    luaL_checktype(L, idx, LUA_TTABLE);
    rlua_setfieldLONG(L, idx, "x", pt.x);
    rlua_setfieldLONG(L, idx, "y", pt.y);
}

static void rlua_toPOINT(lua_State* L, int idx, POINT* pt)
{
    luaL_checktype(L, idx, LUA_TTABLE);
    pt->x = rlua_getfieldLONG(L, idx, "x");
    pt->y = rlua_getfieldLONG(L, idx, "y");
}

REGISTER_CHECK_STRUCT(POINT)

// FILETIME

inline void rlua_fromFILETIME(lua_State* L, int idx, const FILETIME ft)
{
    luaL_checktype(L, idx, LUA_TTABLE);
    rlua_setfieldLONG(L, idx, "dwLowDateTime", ft.dwLowDateTime);
    rlua_setfieldLONG(L, idx, "dwHighDateTime", ft.dwHighDateTime);
}

static void rlua_toFILETIME(lua_State* L, int idx, FILETIME* ft)
{
    luaL_checktype(L, idx, LUA_TTABLE);
    ft->dwLowDateTime = rlua_getfieldLONG(L, idx, "dwLowDateTime");
    ft->dwHighDateTime = rlua_getfieldLONG(L, idx, "dwHighDateTime");
}

REGISTER_CHECK_STRUCT(FILETIME)

// MSG

inline void rlua_fromMSG(lua_State* L, int idx, const MSG* m)
{
    luaL_checktype(L, idx, LUA_TTABLE);
    rlua_setfieldHWND(L, idx, "hwnd", m->hwnd);
    rlua_setfieldUINT(L, idx, "message", m->message);
    rlua_setfieldWPARAM(L, idx, "wParam", m->wParam);
    rlua_setfieldLPARAM(L, idx, "lParam", m->lParam);
    rlua_setfieldDWORD(L, idx, "time", m->time);
    rlua_setfieldPOINT(L, idx, "pt", m->pt);
}

inline void rlua_toMSG(lua_State* L, int idx, MSG* m)
{
    luaL_checktype(L, idx, LUA_TTABLE);
    m->hwnd = rlua_getfieldHWND(L, idx, "hwnd");
    m->message = rlua_getfieldUINT(L, idx, "message");
    m->wParam = rlua_getfieldWPARAM(L, idx, "wParam");
    m->lParam = rlua_getfieldLPARAM(L, idx, "lParam");
    m->time = rlua_getfieldDWORD(L, idx, "time");
    m->pt = rlua_getfieldPOINT(L, idx, "pt");
}

// WIN32_FIND_DATA

inline void rlua_fromWIN32_FIND_DAT(lua_State* L, int idx, const WIN32_FIND_DATA* FindFileData)
{
    luaL_checktype(L, idx, LUA_TTABLE);
    rlua_setfieldDWORD(L, idx, "dwFileAttributes", FindFileData->dwFileAttributes);
    rlua_setfieldFILETIME(L, idx, "ftCreationTime", FindFileData->ftCreationTime);
    rlua_setfieldFILETIME(L, idx, "ftLastAccessTime", FindFileData->ftLastAccessTime);
    rlua_setfieldFILETIME(L, idx, "ftCreationTime", FindFileData->ftLastWriteTime);
    rlua_setfieldDWORD(L, idx, "nFileSizeHigh", FindFileData->nFileSizeHigh);
    rlua_setfieldDWORD(L, idx, "nFileSizeLow", FindFileData->nFileSizeLow);
    rlua_setfieldDWORD(L, idx, "dwReserved0", FindFileData->dwReserved0);
    rlua_setfieldDWORD(L, idx, "dwReserved1", FindFileData->dwReserved1);
    rlua_setfieldstring(L, idx, "cFileName", FindFileData->cFileName);
    rlua_setfieldstring(L, idx, "cAlternateFileName", FindFileData->cAlternateFileName);
}
