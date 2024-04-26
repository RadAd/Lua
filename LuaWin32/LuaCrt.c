#include "lua.h"
#include "lauxlib.h"

#include <Windows.h>
#include <io.h>
#include <fcntl.h>

#include "LuaUtils.h"
#include "LuaWin32Types.h"

typedef luaL_Stream LStream;
#define rlua_checkLStream(L, arg)	((LStream *)luaL_checkudata(L, arg, LUA_FILEHANDLE))
#define isclosed(p)	((p)->closef == NULL)

static FILE* rlua_checkFILEp(lua_State* L, int arg) {
    LStream* p = rlua_checkLStream(L, arg);
    if (luai_unlikely(isclosed(p)))
        luaL_error(L, "attempt to use a closed file");
    lua_assert(p->f);
    return p->f;
}

static int l_FileNo(lua_State* L)
{
    int arg = 0;
    FILE* const f = rlua_checkFILEp(L, ++arg);

    const int n = _fileno(f);

    const int rt = lua_gettop(L);
    lua_pushinteger(L, n);
    return lua_gettop(L) - rt;
}

static int l_SetMode(lua_State* L)
{
    int arg = 0;
    const int fd = rlua_checkint(L, ++arg);
    const int mode = rlua_checkint(L, ++arg);

    const int prev_mode = _setmode(fd, mode);

    const int rt = lua_gettop(L);
    lua_pushinteger(L, prev_mode);
    return lua_gettop(L) - rt;
}

const struct lua_RegInt fcntl[] = {
  { "_O_RDONLY",    _O_RDONLY   },
  { "_O_WRONLY",    _O_WRONLY   },
  { "_O_RDWR",      _O_RDWR     },
  { "_O_APPEND",    _O_APPEND   },
  { "_O_CREAT",     _O_CREAT    },
  { "_O_TRUNC",     _O_TRUNC    },
  { "_O_EXCL",      _O_EXCL     },
  { "_O_TEXT",      _O_TEXT     },
  { "_O_BINARY",    _O_BINARY   },
  { "_O_WTEXT",     _O_WTEXT    },
  { "_O_U16TEXT",   _O_U16TEXT  },
  { "_O_U8TEXT",    _O_U8TEXT   },
  { "_O_RAW",       _O_RAW      },

  { NULL, 0 },
};

extern const struct luaL_Reg crtlib[] = {
  { "_fileno", l_FileNo },
  { "_setmode", l_SetMode },

  { NULL, 0 },
};
