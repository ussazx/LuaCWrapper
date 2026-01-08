#pragma once
#include "../LuaCWrapper/LuaGlobalReflect.h"
#include <iostream>

inline int f1(int a, int b)
{
	return a + b;
}

//使用std::tuple可以返回多个值 std::tuple can return multiple values
inline std::tuple<int, float, bool, const char*, void*> f2(int a, float b, bool c, const char* d, void* e)
{
	return { a, b, c, d, e };
}
Lua_global_add_cfunc(f2) //添加到全局环境

//使用LuaReturn作为lua返回值，方法返回类型必须为void
//using LuaReturn for return value for lua, return type must be void
inline void f3(LuaReturn& ret, int a, float b, bool c, const char* d, void* e)
{
	ret.Push(a);
	ret.Push(b);
	ret.Push(c);
	ret.Push(d);
	ret.Push(e);
}
Lua_global_add_cfunc(f3) //添加到全局环境 add function to global

//入参为lua table时，须使用LuaIdx类型
//if param is a lua table, param type should be LuaIdx 
inline size_t f4(LuaIdx t, size_t a, size_t b)
{
	size_t n;
	t.GetValue(a, &n);
	std::cout << "get t[a]: " << n << std::endl;
	
	t.SetValue(a, b); //t[a] = b
	return a + b;
}
Lua_global_add_cfunc(f4)

inline void FunctionTest(LuaState& lua)
{
	//将包装后的f1 C++方法赋给f1 assign f1 with wrapped f1 C++ method
	lua.SetValue("f1", Lua_cf(f1));

	//在lua中调用f1 call f1 in lua
	lua.Run("print(f1(1, 1))");

	int n;
	//在C++调用f1, n为返回值 call f1 in C++， n for return value
	lua.GetValue("f1", LuaCall(1, 2), &n);
	std::cout << n << std::endl << std::endl;

	//将n作为userdata赋给n assign n width n as userdata
	lua.SetValue("n", &n);

	//call f2 in lua
	lua.Run("print(f2(1, 2.5, true, 'abcde', n))");

	//call f3 in lua
	lua.Run("print(f3(1, 2.5, true, 'abcde', n))");

	//call f4 in lua
	lua.Run("a = {[1] = 1} f4(a, 1, 5) print(a[1])");

	//define function a.f3
	lua.Run("function a.f3(a, b, c, d, e) return a, b, c, d, e end");

	//call a.f3 in C++
	int r1;
	float r2;
	bool r3;
	const char* r4;
	void* r5;
	lua.GetValue("a", "f3", LuaCall(2, 3.5, false, "wwwww", &n), &r1, &r2, &r3, &r4, &r5);

	//define function a:f4
	lua.Run("function a:f4(a, b, c, d, e) print('a', self) return a, b, c, d, e end");

	lua.Run("a2 = {a = a}");

	//call a2.a:f4 in C++
	lua.GetValue("a2", "a", "f4", LuaObjCall(2, 3.5, false, "wwwww", &n), &r1, &r2, &r3, &r4, &r5);
}