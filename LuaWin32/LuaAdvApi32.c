#include "lua.h"

#include <Windows.h>

#include "Buffer.h"
#include "LuaUtils.h"
#include "LuaWin32Types.h"

static int l_RegCloseKey(lua_State* L)
{
    int arg = 0;
    HKEY hKey = rlua_checkHKEY(L, ++arg);

    LSTATUS s = RegCloseKey(hKey);

    const int rt = lua_gettop(L);
    if (s == ERROR_SUCCESS)
        rlua_pushBOOL(L, TRUE);
    else
    {
        SetLastError(s);
        rlua_pushBOOL(L, FALSE);
    }
    return lua_gettop(L) - rt;
}

static int l_RegEnumKeyEx(lua_State* L)
{
    int arg = 0;
    HKEY hKey = rlua_checkHKEY(L, ++arg);
    DWORD dwIndex = rlua_checkDWORD(L, ++arg);

    static CharBuffer cbName;
    if (cbName.size == 0)
        cbName = CharBufferCreate();

    static CharBuffer cbClass;
    if (cbClass.size == 0)
        cbClass = CharBufferCreate();

    DWORD dwNameSize = cbName.size;
    DWORD dwClassSize = cbClass.size;
    FILETIME ftLastWriteTime;

    LSTATUS s = RegEnumKeyEx(hKey, dwIndex, cbName.str, &dwNameSize, NULL, cbClass.str, &dwClassSize, &ftLastWriteTime);

    const int rt = lua_gettop(L);
    if (s == ERROR_SUCCESS)
    {
        rlua_pushstring(L, cbName.str);
        rlua_pushstring(L, cbClass.str);
        rlua_pushFILETIME(L, ftLastWriteTime);
    }
    else
    {
        SetLastError(s);
        lua_pushnil(L);
    }
    return lua_gettop(L) - rt;
}

static int l_RegOpenKey(lua_State* L)
{
    int arg = 0;
    HKEY hKey = rlua_checkHKEY(L, ++arg);
    const char* lpSubKey = luaL_checkstring(L, ++arg);

    HKEY hKeyResult;
    LSTATUS s = RegOpenKey(hKey, lpSubKey, &hKeyResult);

    const int rt = lua_gettop(L);
    if (s == ERROR_SUCCESS)
        rlua_pushHKEY(L, hKeyResult);
    else
    {
        SetLastError(s);
        lua_pushnil(L);
    }
    return lua_gettop(L) - rt;
}

extern const struct luaL_Reg advapi32lib[] = {
  { "RegCloseKey", l_RegCloseKey },
  { "RegEnumKeyEx", l_RegEnumKeyEx },
  { "RegOpenKey", l_RegOpenKey },

  { NULL, 0 },
};
