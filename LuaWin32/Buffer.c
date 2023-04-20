#include "Buffer.h"

#include <stdlib.h>

#include <lua.h>
#include <lauxlib.h>

CharBuffer CharBufferCreateSize(DWORD size)
{
    CharBuffer cb;
    cb.size = size;
    cb.str = malloc(cb.size * sizeof(char));
    return cb;
}

void CharBufferDelete(CharBuffer* pcb)
{
    free(pcb->str);
    pcb->str = NULL;
}

void CharBufferIncreaseSize(lua_State* L, CharBuffer* pcb, DWORD size)
{
    if (size > pcb->size)
    {
        CharBuffer cb = *pcb;
        while (cb.size < size)
            cb.size *= 2;
        cb.str = realloc(pcb->str, cb.size * sizeof(char));
        if (cb.str == NULL)
            /*return*/ luaL_error(L, "CharBufferIncreaseSize failed\n");
        *pcb = cb;
    }
}
