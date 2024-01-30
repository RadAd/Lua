unsigned long readbytes = 0;
int readresult = 0;

#define lua_initreadline(L)  ((void)L)
#define lua_readline(L,b,p) \
        ((void)L, fputs(p, stdout), fflush(stdout),  /* show prompt */ \
        readresult = ReadConsole(GetStdHandle(STD_INPUT_HANDLE), b, LUA_MAXINPUT, &readbytes, NULL), b[readbytes] = '\0', readresult)  /* get line */
#define lua_saveline(L,line)	{ (void)L; (void)line; }
#define lua_freeline(L,b)	{ (void)L; (void)b; }
