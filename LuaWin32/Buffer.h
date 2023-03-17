#pragma once
//#include <Windows.h>
typedef unsigned long       DWORD;
typedef struct lua_State lua_State;

typedef struct CharBuffer
{
    DWORD size;
    char* str;
} CharBuffer;

CharBuffer CharBufferCreate();
void CharBufferDelete(CharBuffer* pcb);
void CharBufferIncreaseSize(lua_State* L, CharBuffer* pcb, DWORD size);
