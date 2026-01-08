#pragma once
#include "../LuaWrapper/LuaUtility.h"
#include <iostream>
#ifdef WIN32
#include <windows.h>
#endif

inline void fs(LString s)
{
#ifdef WIN32
	OutputDebugString(s.c_str());
	OutputDebugString(L"\n");
#endif
}
Lua_global_add_cfunc(fs)

inline void LStringTest(LuaState& lua)
{
	std::wstring_convert<std::codecvt_utf8<wchar_t>> conv;
	
	lua.Run(conv.to_bytes(L"fs('一二三四五12345abcde')").c_str());

	lua.Run(conv.to_bytes(L"s = LString('一二三四五12345abcde') fs(s)").c_str());

	lua.Run(conv.to_bytes(L"s = LString('') s = s .. '一二三四五12345abcde' fs(s)").c_str());

	lua.Run("s = s:substr(5, 5) print(s:utf8())");
}
