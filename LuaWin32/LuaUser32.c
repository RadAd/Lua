#include <lua.h>
#include <Windows.h>

#include "Buffer.h"
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
    const ATOM class_atom = (ATOM) GetClassLongPtr(hwnd, GCW_ATOM);
    const WndProcSet* const pwnd_proc_set = find_wnd_proc_set(class_atom);

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
    const HWND hwnd = rlua_checkHWND(L, ++arg);

    PAINTSTRUCT ps;
    ZeroMemory(&ps, sizeof(ps));
    const HDC hdc = BeginPaint(hwnd, &ps);

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
    LPCSTR const lpszClassName = rlua_getfieldstring(L, o, "lpszClassName");
    LPCSTR const lpszWindowName = rlua_getfieldstring(L, o, "name");
    const DWORD style = rlua_getfieldDWORD(L, o, "style");
    const DWORD exstyle = rlua_getfieldoptDWORD(L, o, "exstyle", 0);
    const int x = rlua_getfieldoptint(L, o, "x", CW_USEDEFAULT);
    const int y = rlua_getfieldoptint(L, o, "y", CW_USEDEFAULT);
    const int w = rlua_getfieldoptint(L, o, "w", CW_USEDEFAULT);
    const int h = rlua_getfieldoptint(L, o, "h", CW_USEDEFAULT);

    const HWND hwnd = CreateWindowEx(
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
    const HWND hwnd = rlua_checkHWND(L, ++arg);
    const UINT uMsg = rlua_checkUINT(L, ++arg);
    const WPARAM wParam = rlua_checkWPARAM(L, ++arg);
    const LPARAM lParam = rlua_checkLPARAM(L, ++arg);

    const LRESULT r = DefWindowProc(hwnd, uMsg, wParam, lParam);

    const int rt = lua_gettop(L);
    lua_pushinteger(L, r);
    return lua_gettop(L) - rt;
}

static int l_DispatchMessage(lua_State* L)
{
    g_L = L;

    int arg = 0;
    // TODO Use MSG msg = rlua_checkMSG(L, ++arg);
    MSG msg;
    ZeroMemory(&msg, sizeof(msg));
    const int idxMSG = ++arg;
    rlua_toMSG(L, idxMSG, &msg);

    const LRESULT r = DispatchMessage(&msg);

    //rlua_fromMSG(L, idxMSG, &msg);

    g_L = NULL;
    const int rt = lua_gettop(L);
    rlua_pushLRESULT(L, r);
    return lua_gettop(L) - rt;
}

static int l_EndPaint(lua_State* L)
{
    int arg = 0;
    const HWND hwnd = rlua_checkHWND(L, ++arg);
    const int psindex = ++arg;

    PAINTSTRUCT ps;
    ZeroMemory(&ps, sizeof(ps));
    ps.hdc = rlua_getfieldHDC(L, psindex, "hdc");
    ps.fErase = rlua_getfieldBOOL(L, psindex, "fErase");
    ps.fRestore = rlua_getfieldBOOL(L, psindex, "fRestore");
    ps.fIncUpdate = rlua_getfieldBOOL(L, psindex, "fIncUpdate");
    ps.rcPaint = rlua_getfieldRECT(L, psindex, "rcPaint");

    const BOOL r = EndPaint(hwnd, &ps);

    const int rt = lua_gettop(L);
    rlua_pushBOOL(L, r);
    return lua_gettop(L) - rt;
}

static BOOL WINAPI EnumWindowsProc(HWND hWnd, LPARAM lParam)
{
    lua_State* const L = (lua_State*) lParam;
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
    const HDC hdc = rlua_checkHDC(L, ++arg);
    const RECT rc = rlua_checkRECT(L, ++arg);
    const HBRUSH hbrush = rlua_checkHBRUSH(L, ++arg);

    const int r = FillRect(hdc, &rc, hbrush);

    const int rt = lua_gettop(L);
    rlua_pushint(L, r);
    return lua_gettop(L) - rt;
}

static int l_FindWindow(lua_State* L)
{
    int arg = 0;
    const char* const lpClassName = rlua_checkstringornil(L, ++arg);
    const char* const lpWindowName = rlua_checkstringornil(L, ++arg);

    const HWND hWnd = FindWindow(lpClassName, lpWindowName);

    const int rt = lua_gettop(L);
    rlua_pushHWND(L, hWnd);
    return lua_gettop(L) - rt;
}

static int l_GetClassName(lua_State* L)
{
    int arg = 0;
    const HWND hWnd = rlua_checkHWND(L, ++arg);

    static CharBuffer cb;
    if (cb.size == 0)
        cb = CharBufferCreate();

    int len;
    while ((DWORD) (len = GetClassName(hWnd, cb.str, cb.size)) == (cb.size - 1))
        CharBufferIncreaseSize(L, &cb, cb.size + 1);

    const int rt = lua_gettop(L);
    if (len == 0 && GetLastError() != ERROR_SUCCESS)
        lua_pushnil(L);
    else
        lua_pushstring(L, cb.str);
    return lua_gettop(L) - rt;
}

static int l_GetForegroundWindow(lua_State* L)
{
    const HWND hwnd = GetForegroundWindow();

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

    // TODO Use MSG msg = rlua_checkMSG(L, ++arg);
    MSG msg;
    ZeroMemory(&msg, sizeof(msg));
    const BOOL r = GetMessage(&msg, hwnd, wMsgFilterMin, wMsgFilterMax);
    rlua_fromMSG(L, msgidx, &msg);

    g_L = NULL;
    const int rt = lua_gettop(L);
    rlua_pushBOOL(L, r);
    return lua_gettop(L) - rt;
}

static int l_GetWindowLongPtr(lua_State* L)
{
    int arg = 0;
    const HWND hWnd = rlua_checkHWND(L, ++arg);
    const int nIndex = rlua_checkint(L, ++arg);

    const LONG_PTR lp = GetWindowLongPtr(hWnd, nIndex);

    const int rt = lua_gettop(L);
    rlua_pushLONG_PTR(L, lp);
    return lua_gettop(L) - rt;
}

static int l_GetWindowRect(lua_State* L)
{
    int arg = 0;
    const HWND hWnd = rlua_checkHWND(L, ++arg);
    const int rect_idx = ++arg;
    RECT rc; //rlua_checkRECT(L, rect_idx);
    ZeroMemory(&rc, sizeof(rc));

    const BOOL r = GetWindowRect(hWnd, &rc);

    if (r)
        rlua_fromRECT(L, rect_idx, &rc);

    const int rt = lua_gettop(L);
    rlua_pushBOOL(L, r);
    return lua_gettop(L) - rt;
}

static int l_GetWindowText(lua_State* L)
{
    int arg = 0;
    const HWND hWnd = rlua_checkHWND(L, ++arg);

    const int lenb = GetWindowTextLength(hWnd);
    if (lenb == 0)
    {
        const int rt = lua_gettop(L);
        if (GetLastError() != ERROR_SUCCESS)
            lua_pushnil(L);
        else
            lua_pushstring(L, "");
        return lua_gettop(L) - rt;
    }

    static CharBuffer cb;
    if (cb.size == 0)
        cb = CharBufferCreate();

    CharBufferIncreaseSize(L, &cb, lenb + 1);

    const int len = GetWindowText(hWnd, cb.str, cb.size);

    const int rt = lua_gettop(L);
    if (len == 0 && GetLastError() != ERROR_SUCCESS)
        lua_pushnil(L);
    else
        lua_pushstring(L, cb.str);
    return lua_gettop(L) - rt;
}

static int l_MessageBox(lua_State* L)
{
    int arg = 0;
    const HWND hwnd = rlua_checkHWND(L, ++arg);
    const char* const lpText = rlua_checkstring(L, ++arg);
    const char* const lpCaption = rlua_checkstring(L, ++arg);
    const UINT uType = rlua_checkUINT(L, ++arg);

    const int r = MessageBox(hwnd, lpText, lpCaption, uType);

    const int rt = lua_gettop(L);
    rlua_pushint(L, r);
    return lua_gettop(L) - rt;
}

static int l_PostQuitMessage(lua_State* L)
{
    const int nExitCode = rlua_checkint(L, 1);

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
    ATOM class_atom = RegisterClass(&wndclass);

    WndProcSet* pwnd_proc_set = find_wnd_proc_set(0);
    pwnd_proc_set->class = class_atom;
    lua_getfield(L, wndclassidx, "wnd_proc");
    pwnd_proc_set->ref = luaL_ref(L, LUA_REGISTRYINDEX);

    const int rt = lua_gettop(L);
    rlua_pushATOM(L, class_atom);
    return lua_gettop(L) - rt;
}

static int l_SetWindowPos(lua_State* L)
{
    int arg = 0;
    const HWND hWnd = rlua_checkHWND(L, ++arg);
    const HWND hWndInsertAfter = rlua_checkHWND(L, ++arg);
    const RECT rc = rlua_checkRECT(L, ++arg);
    const UINT flags = rlua_checkUINT(L, ++arg);

    const BOOL r = SetWindowPos(hWnd, hWndInsertAfter, rc.left, rc.top, rc.right - rc.left, rc.bottom - rc.top, flags);

    const int rt = lua_gettop(L);
    rlua_pushBOOL(L, r);
    return lua_gettop(L) - rt;
}

static int l_TranslateMessage(lua_State* L)
{
    const int r1 = lua_gettop(L);
    int arg = 0;
    // TODO Use MSG msg = rlua_checkMSG(L, ++arg);
    MSG msg;
    ZeroMemory(&msg, sizeof(msg));
    const int idxMSG = ++arg;
    rlua_toMSG(L, idxMSG, &msg);
    const int r2 = lua_gettop(L);

    const BOOL r = TranslateMessage(&msg);

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
  { "FindWindow", l_FindWindow },
  { "GetClassName", l_GetClassName },
  { "GetForegroundWindow", l_GetForegroundWindow },
  { "GetMessage", l_GetMessage },
  { "GetWindowLongPtr", l_GetWindowLongPtr },
  { "GetWindowRect", l_GetWindowRect },
  { "GetWindowText", l_GetWindowText },
  { "MessageBox", l_MessageBox },
  { "PostQuitMessage", l_PostQuitMessage },
  { "RegisterClass", l_RegisterClass },
  { "SetWindowPos", l_SetWindowPos },
  { "TranslateMessage", l_TranslateMessage },

  { NULL, 0 },
};
