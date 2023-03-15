#include "lua.h"

#include <Windows.h>

#include "LuaUtils.h"
#include "LuaWin32Types.h"

extern HINSTANCE g_hInstance;

static lua_State* g_L = NULL;

typedef struct WndProcSet
{
    ATOM class;
    int ref;
} WndProcSet;

WndProcSet wnd_proc_set[10];

inline WndProcSet* find_wnd_proc_set(ATOM class_atom)
{
    for (int i = 0; i < ARRAYSIZE(wnd_proc_set); ++i)
    {
        if (wnd_proc_set[i].class == class_atom)
            return &wnd_proc_set[i];
    }
    return NULL;
}

static LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    const ATOM class_atom = (ATOM) GetClassLongPtrA(hwnd, GCW_ATOM);
    const WndProcSet* pwnd_proc_set = find_wnd_proc_set(class_atom);

    lua_State* L = g_L;
    lua_rawgeti(L, LUA_REGISTRYINDEX, pwnd_proc_set->ref);

    rlua_pushHWND(L, hwnd);
    rlua_pushUINT(L, uMsg);
    rlua_pushWPARAM(L, wParam);
    rlua_pushLPARAM(L, lParam);

    lua_call(L, 4, 1);

    LRESULT r = rlua_checkLRESULT(L, -1);
    lua_pop(L, 1);
    return r;
}

static int l_BeginPaint(lua_State* L)
{
    int arg = 0;
    HWND hwnd = rlua_checkHWND(L, ++arg);

    PAINTSTRUCT ps;
    ZeroMemory(&ps, sizeof(ps));
    HDC hdc = BeginPaint(hwnd, &ps);

    const int rt = lua_gettop(L);
    lua_newtable(L);
    rlua_setfieldHDC(L, -1, "hdc", ps.hdc);
    rlua_setfieldBOOL(L, -1, "fErase", ps.fErase);
    rlua_setfieldRECT(L, -1, "rcPaint", ps.rcPaint);
    rlua_setfieldBOOL(L, -1, "fRestore", ps.fRestore);
    rlua_setfieldBOOL(L, -1, "fIncUpdate", ps.fIncUpdate);
    return lua_gettop(L) - rt;
}

static int l_CreateWindowEx(lua_State* L)
{
    g_L = L;

    const int o = 1;
    LPCSTR lpszClassName = rlua_getfieldstring(L, o, "lpszClassName");
    LPCSTR lpszWindowName = rlua_getfieldstring(L, o, "name");
    DWORD style = rlua_getfieldDWORD(L, o, "style");
    DWORD exstyle = rlua_getfieldoptDWORD(L, o, "exstyle", 0);
    int x = rlua_getfieldoptint(L, o, "x", CW_USEDEFAULT);
    int y = rlua_getfieldoptint(L, o, "y", CW_USEDEFAULT);
    int w = rlua_getfieldoptint(L, o, "w", CW_USEDEFAULT);
    int h = rlua_getfieldoptint(L, o, "h", CW_USEDEFAULT);

    HWND hwnd = CreateWindowExA(
        exstyle,
        lpszClassName,
        lpszWindowName,
        style,
        x, y, w, h,

        NULL,       // Parent window
        NULL,       // Menu
        g_hInstance,  // Instance handle
        NULL        // Additional application data
    );

    g_L = NULL;
    rlua_pushHWND(L, hwnd);
    return 1;
}

static int l_DefWindowProc(lua_State* L)
{
    int arg = 0;
    HWND hwnd = rlua_checkHWND(L, ++arg);
    UINT uMsg = rlua_checkUINT(L, ++arg);
    WPARAM wParam = rlua_checkWPARAM(L, ++arg);
    LPARAM lParam = rlua_checkLPARAM(L, ++arg);

    LRESULT r = DefWindowProcA(hwnd, uMsg, wParam, lParam);

    const int rt = lua_gettop(L);
    lua_pushinteger(L, r);
    return lua_gettop(L) - rt;
}

static int l_DispatchMessage(lua_State* L)
{
    g_L = L;

    int arg = 0;
    MSG msg;
    ZeroMemory(&msg, sizeof(msg));
    const int idxMSG = ++arg;
    rlua_toMSG(L, idxMSG, &msg);

    LRESULT r = DispatchMessageA(&msg);

    //rlua_fromMSG(L, idxMSG, &msg);

    g_L = NULL;
    const int rt = lua_gettop(L);
    rlua_pushLRESULT(L, r);
    return lua_gettop(L) - rt;
}

static int l_EndPaint(lua_State* L)
{
    int arg = 0;
    HWND hwnd = rlua_checkHWND(L, ++arg);
    const int psindex = ++arg;

    PAINTSTRUCT ps;
    ZeroMemory(&ps, sizeof(ps));
    ps.hdc = rlua_getfieldHDC(L, psindex, "hdc");
    ps.fErase = rlua_getfieldBOOL(L, psindex, "fErase");
    ps.fRestore = rlua_getfieldBOOL(L, psindex, "fRestore");
    ps.fIncUpdate = rlua_getfieldBOOL(L, psindex, "fIncUpdate");
    ps.rcPaint = rlua_getfieldRECT(L, psindex, "rcPaint");

    BOOL r = EndPaint(hwnd, &ps);

    const int rt = lua_gettop(L);
    rlua_pushBOOL(L, r);
    return lua_gettop(L) - rt;
}

static BOOL WINAPI EnumWindowsProc(HWND hWnd, LPARAM lParam)
{
    lua_State* L = (lua_State*) lParam;
    rlua_pushHWND(L, hWnd);
    lua_appendarray(L, -2);
    return TRUE;
}

static int l_EnumWindows(lua_State* L)
{
    lua_newarray(L);
    EnumWindows(EnumWindowsProc, (LPARAM) L);
    return 1;
}

static int l_FillRect(lua_State* L)
{
    int arg = 0;
    HDC hdc = rlua_checkHDC(L, ++arg);
    RECT rc = rlua_checkRECT(L, ++arg);
    HBRUSH hbrush = rlua_checkHBRUSH(L, ++arg);

    int r = FillRect(hdc, &rc, hbrush);

    const int rt = lua_gettop(L);
    rlua_pushint(L, r);
    return lua_gettop(L) - rt;
}

static int l_GetClassName(lua_State* L)
{
    int arg = 0;
    HWND hWnd = rlua_checkHWND(L, ++arg);

    char buffer[1024];
    if (GetClassNameA(hWnd, buffer, ARRAYSIZE(buffer)))
        lua_pushstring(L, buffer);
    else
        lua_pushnil(L);
    return 1;
}

static int l_GetForegroundWindow(lua_State* L)
{
    HWND hwnd = GetForegroundWindow();

    const int rt = lua_gettop(L);
    rlua_pushHWND(L, hwnd);
    return lua_gettop(L) - rt;
}

static int l_GetMessage(lua_State* L)
{
    g_L = L;

    int arg = 0;
    const int msgidx = ++arg;
    luaL_checktype(L, msgidx, LUA_TTABLE);
    const HWND hwnd = rlua_optHWND(L, ++arg, NULL);
    const UINT wMsgFilterMin = rlua_optUINT(L, ++arg, 0);
    const UINT wMsgFilterMax = rlua_optUINT(L, ++arg, 0);

    MSG msg;
    ZeroMemory(&msg, sizeof(msg));
    BOOL r = GetMessageA(&msg, hwnd, wMsgFilterMin, wMsgFilterMax);
    rlua_fromMSG(L, msgidx, &msg);

    g_L = NULL;
    const int rt = lua_gettop(L);
    rlua_pushBOOL(L, r);
    return lua_gettop(L) - rt;
}

static int l_GetWindowLongPtr(lua_State* L)
{
    int arg = 0;
    HWND hWnd = rlua_checkHWND(L, ++arg);
    int nIndex = rlua_checkint(L, ++arg);

    LONG_PTR lp = GetWindowLongPtr(hWnd, nIndex);

    const int rt = lua_gettop(L);
    rlua_pushLONG_PTR(L, lp);
    return lua_gettop(L) - rt;
}

static int l_GetWindowText(lua_State* L)
{
    int arg = 0;
    HWND hWnd = rlua_checkHWND(L, ++arg);

    char buffer[1024];
    int len = GetWindowTextA(hWnd, buffer, ARRAYSIZE(buffer));
    // TODO check GetLastError ?

    const int rt = lua_gettop(L);
    lua_pushstring(L, buffer);
    return lua_gettop(L) - rt;
}

static int l_MessageBox(lua_State* L)
{
    int arg = 0;
    HWND hwnd = rlua_checkHWND(L, ++arg);
    const char* lpText = rlua_checkstring(L, ++arg);
    const char* lpCaption = rlua_checkstring(L, ++arg);
    UINT uType = rlua_checkUINT(L, ++arg);

    int r = MessageBoxA(hwnd, lpText, lpCaption, uType);

    const int rt = lua_gettop(L);
    rlua_pushint(L, r);
    return lua_gettop(L) - rt;
}

static int l_PostQuitMessage(lua_State* L)
{
    int nExitCode = rlua_checkint(L, 1);

    PostQuitMessage(nExitCode);

    const int rt = lua_gettop(L);
    return lua_gettop(L) - rt;
}

static int l_RegisterClass(lua_State* L)
{
    int arg = 0;
    const int wndclassidx = ++arg;

    WNDCLASSA wndclass;
    ZeroMemory(&wndclass, sizeof(wndclass));

    wndclass.lpfnWndProc = WindowProc;
    wndclass.hInstance = g_hInstance;

    wndclass.style = rlua_getfieldUINT(L, wndclassidx, "style");
    //WNDPROC     lpfnWndProc;
    //int         cbClsExtra;
    //int         cbWndExtra;
    //HINSTANCE   hInstance;
    wndclass.hIcon = rlua_getfieldoptHICON(L, wndclassidx, "hIcon", NULL);
    wndclass.hCursor = rlua_getfieldoptHCURSOR(L, wndclassidx, "hCursor", NULL);
    wndclass.hbrBackground = rlua_getfieldoptHBRUSH(L, wndclassidx, "hbrBackground", NULL);
    //LPCWSTR     lpszMenuName;
    wndclass.lpszClassName = rlua_getfieldstring(L, wndclassidx, "lpszClassName");
    ATOM class_atom = RegisterClassA(&wndclass);

    WndProcSet* pwnd_proc_set = find_wnd_proc_set(0);
    pwnd_proc_set->class = class_atom;
    lua_getfield(L, wndclassidx, "wnd_proc");
    pwnd_proc_set->ref = luaL_ref(L, LUA_REGISTRYINDEX);

    const int rt = lua_gettop(L);
    rlua_pushATOM(L, class_atom);
    return lua_gettop(L) - rt;
}

static int l_TranslateMessage(lua_State* L)
{
    const int r1 = lua_gettop(L);
    int arg = 0;
    MSG msg;
    ZeroMemory(&msg, sizeof(msg));
    const int idxMSG = ++arg;
    rlua_toMSG(L, idxMSG, &msg);
    const int r2 = lua_gettop(L);

    BOOL r = TranslateMessage(&msg);

    const int r3 = lua_gettop(L);

    rlua_fromMSG(L, idxMSG, &msg);

    const int rt = lua_gettop(L);
    rlua_pushBOOL(L, r);
    return lua_gettop(L) - rt;
}


const struct luaL_Reg user32lib[] = {
  { "BeginPaint", l_BeginPaint },
  { "CreateWindowEx", l_CreateWindowEx },
  { "DefWindowProc", l_DefWindowProc },
  { "DispatchMessage", l_DispatchMessage },
  { "EndPaint", l_EndPaint },
  { "EnumWindows", l_EnumWindows },
  { "FillRect", l_FillRect },
  { "GetClassName", l_GetClassName },
  { "GetForegroundWindow", l_GetForegroundWindow },
  { "GetMessage", l_GetMessage },
  { "GetWindowLongPtr", l_GetWindowLongPtr },
  { "GetWindowText", l_GetWindowText },
  { "MessageBox", l_MessageBox },
  { "PostQuitMessage", l_PostQuitMessage },
  { "RegisterClass", l_RegisterClass },
  { "TranslateMessage", l_TranslateMessage },

  { NULL, 0 },
};
