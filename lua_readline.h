#include "lua/lprefix.h"
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

int MyReadFile(char* buf, int len)
{
    const HANDLE hStdIn = GetStdHandle(STD_INPUT_HANDLE);
    DWORD readbytes = 0;
    const BOOL result = GetFileType(hStdIn) == FILE_TYPE_CHAR
        ? ReadConsole(hStdIn, buf, len, &readbytes, NULL)
        : ReadFile(hStdIn, buf, len, &readbytes, NULL);
    if (result)
        buf[readbytes] = '\0';
    return result;
}

#define lua_initreadline(L)  ((void)L)
#define lua_readline(b,p) \
        ((void) fputs(p, stdout), fflush(stdout),  /* show prompt */ \
        MyReadFile(b, LUA_MAXINPUT), b)  /* get line */
#define lua_saveline(line)	{ (void)line; }
#define lua_freeline(b)	{ (void)b; }
