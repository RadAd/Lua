#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>

#include <Windows.h>

#include "LuaUtils.h"

HINSTANCE g_hInstance = NULL;

#define WIN32_EXPORT __declspec (dllexport)

/*
** Assumes the table is on top of the stack.
*/
static void set_info(lua_State* L)
{
    lua_pushliteral(L, "Copyright (C) 2023 Adam Gates");
    lua_setfield(L, -2, "_COPYRIGHT");
    lua_pushliteral(L, "LuaRadWin32 is a Lua library developed to access the Win32 set of functions");
    lua_setfield(L, -2, "_DESCRIPTION");
    lua_pushliteral(L, "1.0");
    lua_setfield(L, -2, "_VERSION");
}

static const struct lua_RegInt class_styles[] = {
  { "VREDRAW",          CS_VREDRAW          },
  { "HREDRAW",          CS_HREDRAW          },
  { "BLCCLKS",          CS_DBLCLKS          },
  { "WONDC",            CS_OWNDC            },
  { "CLASSDC",          CS_CLASSDC          },
  { "PARENTDC",         CS_PARENTDC         },
  { "NOCLOSE",          CS_NOCLOSE          },
  { "SAVEBITS",         CS_SAVEBITS         },
  { "BYTEALIGNCLIENT",  CS_BYTEALIGNCLIENT  },
  { "BYTEALIGNWINDOW",  CS_BYTEALIGNWINDOW  },
  { "GLOBALCLASS",      CS_GLOBALCLASS      },

  { "IME",              CS_IME              },
  { "DROPSHADOW",       CS_DROPSHADOW       },

  { NULL, 0 },
};

static const struct lua_RegInt window_styles[] = {
  { "OVERLAPPED",       WS_OVERLAPPED       },
  { "POPUP",            WS_POPUP            },
  { "CHILD",            WS_CHILD            },
  { "MINIMIZE",         WS_MINIMIZE         },
  { "VISIBLE",          WS_VISIBLE          },
  { "DISABLED",         WS_DISABLED         },
  { "CLIPSIBLINGS",     WS_CLIPSIBLINGS     },
  { "CLIPCHILDREN",     WS_CLIPCHILDREN     },
  { "MAXIMIZE",         WS_MAXIMIZE         },
  { "CAPTION",          WS_CAPTION          },
  { "BORDER",           WS_BORDER           },
  { "DLGFRAME",         WS_DLGFRAME         },
  { "VSCROLL",          WS_VSCROLL          },
  { "HSCROLL",          WS_HSCROLL          },
  { "SYSMENU",          WS_SYSMENU          },
  { "THICKFRAME",       WS_THICKFRAME       },
  { "GROUP",            WS_GROUP            },
  { "TABSTOP",          WS_TABSTOP          },

  { "MINIMIZEBOX",      WS_MINIMIZEBOX      },
  { "MAXIMIZEBOX",      WS_MAXIMIZEBOX      },

  { "TILED",            WS_TILED            },
  { "ICONIC",           WS_ICONIC           },
  { "SIZEBOX",          WS_SIZEBOX          },
  { "TILEDWINDOW",      WS_TILEDWINDOW      },

  { "OVERLAPPEDWINDOW", WS_OVERLAPPEDWINDOW },
  { "POPUPWINDOW",      WS_POPUPWINDOW      },
  { "CHILDWINDOW",      WS_CHILDWINDOW      },

  { NULL, 0 },
};

static const struct lua_RegInt window_messages[] = {
  { "NULL",             WM_NULL             },
  { "CREATE",           WM_CREATE           },
  { "DESTROY",          WM_DESTROY          },
  { "MOVE",             WM_MOVE             },
  { "SIZE",             WM_SIZE             },
  { "ACTIVATE",         WM_ACTIVATE         },

  { "SETFOCUS",         WM_SETFOCUS         },
  { "KILLFOCUS",        WM_KILLFOCUS        },
  { "ENABLE",           WM_ENABLE           },
  { "SETREDRAW",        WM_SETREDRAW        },
  { "GETTEXT",          WM_GETTEXT          },
  { "GETTEXT",          WM_GETTEXT          },
  { "GETTEXTLENGTH",    WM_GETTEXTLENGTH    },
  { "PAINT",            WM_PAINT            },
  { "CLOSE",            WM_CLOSE            },
  { "QUERYENDSESSION",  WM_QUERYENDSESSION  },
  { "QUERYOPEN",        WM_QUERYOPEN        },
  { "ENDSESSION",       WM_ENDSESSION       },
  { "QUIT",             WM_QUIT             },
  { "ERASEBKGND",       WM_ERASEBKGND       },
  { "SYSCOLORCHANGE",   WM_SYSCOLORCHANGE   },
  { "SHOWWINDOW",       WM_SHOWWINDOW       },
  { "WININICHANGE",     WM_WININICHANGE     },

  { NULL, 0 },
};

static const struct lua_RegInt message_box[] = {
  { "OK",                   MB_OK                   },
  { "OKCANCEL",             MB_OKCANCEL             },
  { "ABORTRETRYIGNORE",     MB_ABORTRETRYIGNORE     },
  { "YESNOCANCEL",          MB_YESNOCANCEL          },
  { "YESNO",                MB_YESNO                },
  { "RETRYCANCEL",          MB_RETRYCANCEL          },
  { "CANCELTRYCONTINUE",    MB_CANCELTRYCONTINUE    },

  { "ICONHAND",             MB_ICONHAND             },
  { "ICONQUESTION",         MB_ICONQUESTION         },
  { "ICONEXCLAMATION",      MB_ICONEXCLAMATION      },
  { "ICONASTERISK",         MB_ICONASTERISK         },
  { "USERICON",             MB_USERICON             },
  { "ICONWARNING",          MB_ICONWARNING          },
  { "ICONERROR",            MB_ICONERROR            },
  { "ICONINFORMATION",      MB_ICONINFORMATION      },
  { "ICONSTOP",             MB_ICONSTOP             },

  { "DEFBUTTON1",           MB_DEFBUTTON1           },
  { "DEFBUTTON2",           MB_DEFBUTTON2           },
  { "DEFBUTTON3",           MB_DEFBUTTON3           },
  { "DEFBUTTON4",           MB_DEFBUTTON4           },

  { "APPLMODAL",            MB_APPLMODAL            },
  { "SYSTEMMODAL",          MB_SYSTEMMODAL          },
  { "TASKMODAL",            MB_TASKMODAL            },
  { "HELP",                 MB_HELP                 },

  { "NOFOCUS",              MB_NOFOCUS              },
  { "SETFOREGROUND",        MB_SETFOREGROUND        },
  { "DEFAULT_DESKTOP_ONLY", MB_DEFAULT_DESKTOP_ONLY },

  { "TOPMOST",              MB_TOPMOST              },
  { "RIGHT",                MB_RIGHT                },
  { "RTLREADING",           MB_RTLREADING           },

  { "SERVICE_NOTIFICATION", MB_SERVICE_NOTIFICATION },

  { "TYPEMASK",             MB_TYPEMASK             },
  { "ICONMASK",             MB_ICONMASK             },
  { "DEFMASK",              MB_DEFMASK              },
  { "MODEMASK",             MB_MODEMASK             },
  { "MISCMASK",             MB_MISCMASK             },

  { NULL, 0 },
};

static const struct lua_RegInt color_index[] = {
  { "SCROLLBAR",            COLOR_SCROLLBAR             },
  { "BACKGROUND",           COLOR_BACKGROUND            },
  { "ACTIVECAPTION",        COLOR_ACTIVECAPTION         },
  { "INACTIVECAPTION",      COLOR_INACTIVECAPTION       },
  { "MENU",                 COLOR_MENU                  },
  { "WINDOW",               COLOR_WINDOW                },
  { "WINDOWFRAME",          COLOR_WINDOWFRAME           },
  { "MENUTEXT",             COLOR_MENUTEXT              },
  { "WINDOWTEXT",           COLOR_WINDOWTEXT            },
  { "CAPTIONTEXT",          COLOR_CAPTIONTEXT           },
  { "ACTIVEBORDER",         COLOR_ACTIVEBORDER          },
  { "INACTIVEBORDER",       COLOR_INACTIVEBORDER        },
  { "APPWORKSPACE",         COLOR_APPWORKSPACE          },
  { "HIGHLIGHT",            COLOR_HIGHLIGHT             },
  { "HIGHLIGHTTEXT",        COLOR_HIGHLIGHTTEXT         },
  { "BTNFACE",              COLOR_BTNFACE               },
  { "BTNSHADOW",            COLOR_BTNSHADOW             },
  { "GRAYTEXT",             COLOR_GRAYTEXT              },
  { "BTNTEXT",              COLOR_BTNTEXT               },
  { "INACTIVECAPTIONTEXT",  COLOR_INACTIVECAPTIONTEXT   },
  { "BTNHIGHLIGHT",         COLOR_BTNHIGHLIGHT          },

#if(WINVER >= 0x0400)
  { "3DDKSHADOW", COLOR_3DDKSHADOW  },
  { "3DLIGHT",    COLOR_3DLIGHT     },
  { "INFOTEXT",   COLOR_INFOTEXT    },
  { "INFOBK",     COLOR_INFOBK      },
#endif /* WINVER >= 0x0400 */

#if(WINVER >= 0x0500)
  { "HOTLIGHT",                 COLOR_HOTLIGHT                  },
  { "GRADIENTACTIVECAPTION",    COLOR_GRADIENTACTIVECAPTION     },
  { "GRADIENTINACTIVECAPTION",  COLOR_GRADIENTINACTIVECAPTION   },
#if(WINVER >= 0x0501)
  { "MENUHILIGHT",  COLOR_MENUHILIGHT   },
  { "MENUBAR",      COLOR_MENUBAR       },
#endif /* WINVER >= 0x0501 */
#endif /* WINVER >= 0x0500 */

#if(WINVER >= 0x0400)
  { "DESKTOP",      COLOR_DESKTOP       },
  { "3DFACE",       COLOR_3DFACE        },
  { "3DSHADOW",     COLOR_3DSHADOW      },
  { "3DHIGHLIGHT",  COLOR_3DHIGHLIGHT   },
  { "3DHILIGHT",    COLOR_3DHILIGHT     },
  { "BTNHILIGHT",   COLOR_BTNHILIGHT    },
#endif /* WINVER >= 0x0400 */

  { NULL, 0 },
};

static const struct lua_RegInt get_window_long_types[] = {
  { "WNDPROC",     GWLP_WNDPROC    },
  { "HINSTANCE",   GWLP_HINSTANCE  },
  { "HWNDPARENT",  GWLP_HWNDPARENT },
  { "STYLE",       GWL_STYLE       },
  { "EXSTYLE",     GWL_EXSTYLE     },
  { "USERDATA",    GWLP_USERDATA   },
  { "ID",          GWLP_ID         },

  { NULL, 0 },
};

static const struct lua_RegInt file_attribute[] = {
  { "READONLY",                 FILE_ATTRIBUTE_READONLY },
  { "HIDDEN",                   FILE_ATTRIBUTE_HIDDEN },
  { "SYSTEM",                   FILE_ATTRIBUTE_SYSTEM },
  { "DIRECTORY",                FILE_ATTRIBUTE_DIRECTORY },
  { "ARCHIVE",                  FILE_ATTRIBUTE_ARCHIVE },
  { "DEVICE",                   FILE_ATTRIBUTE_DEVICE },
  { "NORMAL",                   FILE_ATTRIBUTE_NORMAL },
  { "TEMPORARY",                FILE_ATTRIBUTE_TEMPORARY },
  { "SPARSE_FILE",              FILE_ATTRIBUTE_SPARSE_FILE },
  { "REPARSE_POINT",            FILE_ATTRIBUTE_REPARSE_POINT },
  { "COMPRESSED",               FILE_ATTRIBUTE_COMPRESSED },
  { "OFFLINE",                  FILE_ATTRIBUTE_OFFLINE },
  { "NOT_CONTENT_INDEXED",      FILE_ATTRIBUTE_NOT_CONTENT_INDEXED },
  { "ENCRYPTED",                FILE_ATTRIBUTE_ENCRYPTED },
  { "INTEGRITY_STREAM",         FILE_ATTRIBUTE_INTEGRITY_STREAM },
  { "VIRTUAL",                  FILE_ATTRIBUTE_VIRTUAL },
  { "NO_SCRUB_DATA",            FILE_ATTRIBUTE_NO_SCRUB_DATA },
  { "EA",                       FILE_ATTRIBUTE_EA },
  { "PINNED",                   FILE_ATTRIBUTE_PINNED },
  { "UNPINNED",                 FILE_ATTRIBUTE_UNPINNED },
  { "RECALL_ON_OPEN",           FILE_ATTRIBUTE_RECALL_ON_OPEN },
  { "RECALL_ON_DATA_ACCESS",    FILE_ATTRIBUTE_RECALL_ON_DATA_ACCESS },

  { NULL, 0 },
};

extern const struct luaL_Reg kernel32lib[];
extern const struct luaL_Reg user32lib[];
extern const struct luaL_Reg gdi32lib[];

WIN32_EXPORT int luaopen_lrwin32(lua_State* L)
{
    g_hInstance = GetModuleHandle(NULL); // TODO Capture this DLLs instance handle

    luaL_checkversion(L),
    lua_createtable(L, 0, 100);
    luaL_setfuncs(L, kernel32lib, 0);
    luaL_setfuncs(L, user32lib, 0);
    luaL_setfuncs(L, gdi32lib, 0);
    lua_pushvalue(L, -1);
    set_info(L);
    rlua_newtableinteger(L, class_styles, "class_styles");
    rlua_newtableinteger(L, window_styles, "window_styles");
    rlua_newtableinteger(L, window_messages, "window_messages");
    rlua_newtableinteger(L, message_box, "MB");
    rlua_newtableinteger(L, color_index, "color_index");
    rlua_newtableinteger(L, get_window_long_types, "GWLP");
    rlua_newtableinteger(L, file_attribute, "FILE_ATTRIBUTE");
    return 1;
}
