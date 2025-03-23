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
REGISTER_CHECK_INTEGER(ULONG)
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
REGISTER_CHECK_INTEGER(HKEY)

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
    rlua_to##type(L, idx, &v); \
    return v; \
} \
inline type* rlua_check##type##ornil(lua_State* L, int idx) \
{ \
    if (lua_isnil(L, idx)) \
        return NULL; \
    luaL_checktype(L, idx, LUA_TTABLE); \
    type* v = (type*) malloc(sizeof(type)); \
    if (!v) \
        return NULL; \
    ZeroMemory(v, sizeof(type)); \
    rlua_to##type(L, idx, v); \
    return v; \
} \
inline void rlua_push##type(lua_State* L, const type v) \
{ \
    lua_newtable(L); \
    rlua_from##type(L, -1, &v); \
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

#define REGISTER_SET_FIELD(type, object, field) rlua_setfield##type(L, idx, #field, object->field)
#define REGISTER_GET_FIELD(type, object, field) object->field = rlua_getfield##type(L, idx, #field);

// RECT

inline void rlua_fromRECT(lua_State* L, int idx, const RECT* rc)
{
    luaL_checktype(L, idx, LUA_TTABLE);
    REGISTER_SET_FIELD(LONG, rc, left);
    REGISTER_SET_FIELD(LONG, rc, top);
    REGISTER_SET_FIELD(LONG, rc, right);
    REGISTER_SET_FIELD(LONG, rc, bottom);
}

inline void rlua_toRECT(lua_State* L, int idx, RECT* rc)
{
    luaL_checktype(L, idx, LUA_TTABLE);
    REGISTER_GET_FIELD(LONG, rc, left);
    REGISTER_GET_FIELD(LONG, rc, top);
    REGISTER_GET_FIELD(LONG, rc, right);
    REGISTER_GET_FIELD(LONG, rc, bottom);
}

REGISTER_CHECK_STRUCT(RECT)

// POINT

inline void rlua_fromPOINT(lua_State* L, int idx, const POINT* pt)
{
    luaL_checktype(L, idx, LUA_TTABLE);
    REGISTER_SET_FIELD(LONG, pt, x);
    REGISTER_SET_FIELD(LONG, pt, y);
}

static void rlua_toPOINT(lua_State* L, int idx, POINT* pt)
{
    luaL_checktype(L, idx, LUA_TTABLE);
    REGISTER_GET_FIELD(LONG, pt, x);
    REGISTER_GET_FIELD(LONG, pt, y);
}

REGISTER_CHECK_STRUCT(POINT)

// FILETIME

inline void rlua_fromFILETIME(lua_State* L, int idx, const FILETIME* ft)
{
    luaL_checktype(L, idx, LUA_TTABLE);
    REGISTER_SET_FIELD(LONG, ft, dwLowDateTime);
    REGISTER_SET_FIELD(LONG, ft, dwHighDateTime);
}

static void rlua_toFILETIME(lua_State* L, int idx, FILETIME* ft)
{
    luaL_checktype(L, idx, LUA_TTABLE);
    REGISTER_GET_FIELD(LONG, ft, dwLowDateTime);
    REGISTER_GET_FIELD(LONG, ft, dwHighDateTime);
}

REGISTER_CHECK_STRUCT(FILETIME)

// MSG

inline void rlua_fromMSG(lua_State* L, int idx, const MSG* m)
{
    luaL_checktype(L, idx, LUA_TTABLE);
    REGISTER_SET_FIELD(HWND, m, hwnd);
    REGISTER_SET_FIELD(UINT, m, message);
    REGISTER_SET_FIELD(WPARAM, m, wParam);
    REGISTER_SET_FIELD(LPARAM, m, lParam);
    REGISTER_SET_FIELD(DWORD, m, time);
    REGISTER_SET_FIELD(POINT, m, pt);
}

inline void rlua_toMSG(lua_State* L, int idx, MSG* m)
{
    luaL_checktype(L, idx, LUA_TTABLE);
    REGISTER_GET_FIELD(HWND, m, hwnd);
    REGISTER_GET_FIELD(UINT, m, message);
    REGISTER_GET_FIELD(WPARAM, m, wParam);
    REGISTER_GET_FIELD(LPARAM, m, lParam);
    REGISTER_GET_FIELD(DWORD, m, time);
    REGISTER_GET_FIELD(POINT, m, pt);
}

REGISTER_CHECK_STRUCT(MSG)

// WIN32_FIND_DATA

inline void rlua_fromWIN32_FIND_DATA(lua_State* L, int idx, const WIN32_FIND_DATA* FindFileData)
{
    luaL_checktype(L, idx, LUA_TTABLE);
    REGISTER_SET_FIELD(DWORD, FindFileData, dwFileAttributes);
    REGISTER_SET_FIELD(FILETIME, FindFileData, ftCreationTime);
    REGISTER_SET_FIELD(FILETIME, FindFileData, ftLastAccessTime);
    REGISTER_SET_FIELD(FILETIME, FindFileData, ftLastWriteTime);
    REGISTER_SET_FIELD(DWORD, FindFileData, nFileSizeHigh);
    REGISTER_SET_FIELD(DWORD, FindFileData, nFileSizeLow);
    REGISTER_SET_FIELD(DWORD, FindFileData, dwReserved0);
    REGISTER_SET_FIELD(DWORD, FindFileData, dwReserved1);
    REGISTER_SET_FIELD(string, FindFileData, cFileName);
    REGISTER_SET_FIELD(string, FindFileData, cAlternateFileName);
}

//REGISTER_CHECK_STRUCT(WIN32_FIND_DATA)

// SECURITY_ATTRIBUTES

inline void rlua_fromSECURITY_ATTRIBUTES(lua_State* L, int idx, const SECURITY_ATTRIBUTES* lpSecurityAttributes)
{
    luaL_checktype(L, idx, LUA_TTABLE);
    //REGISTER_SET_FIELD(XXX, lpSecurityAttributes, lpSecurityDescriptor);
    REGISTER_SET_FIELD(BOOL, lpSecurityAttributes, bInheritHandle);
}


inline void rlua_toSECURITY_ATTRIBUTES(lua_State* L, int idx, SECURITY_ATTRIBUTES* lpSecurityAttributes)
{
    luaL_checktype(L, idx, LUA_TTABLE);
    //REGISTER_GET_FIELD(XXX, lpSecurityAttributes, lpSecurityDescriptor);
    REGISTER_GET_FIELD(BOOL, lpSecurityAttributes, bInheritHandle);
}

REGISTER_CHECK_STRUCT(SECURITY_ATTRIBUTES)

// CONSOLE_READCONSOLE_CONTROL

inline void rlua_fromCONSOLE_READCONSOLE_CONTROL(lua_State* L, int idx, const CONSOLE_READCONSOLE_CONTROL* pInputControl)
{
    luaL_checktype(L, idx, LUA_TTABLE);
    REGISTER_SET_FIELD(ULONG, pInputControl, nInitialChars);
    REGISTER_SET_FIELD(DWORD, pInputControl, dwCtrlWakeupMask);
    REGISTER_SET_FIELD(DWORD, pInputControl, dwControlKeyState);
}


inline void rlua_toCONSOLE_READCONSOLE_CONTROL(lua_State* L, int idx, CONSOLE_READCONSOLE_CONTROL* pInputControl)
{
    luaL_checktype(L, idx, LUA_TTABLE);
    REGISTER_GET_FIELD(ULONG, pInputControl, nInitialChars);
    REGISTER_GET_FIELD(DWORD, pInputControl, dwCtrlWakeupMask);
    REGISTER_SET_FIELD(DWORD, pInputControl, dwControlKeyState);
}

REGISTER_CHECK_STRUCT(CONSOLE_READCONSOLE_CONTROL)
