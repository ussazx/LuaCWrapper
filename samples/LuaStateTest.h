#pragma once
#include "../LuaCWrapper/LuaState.h"
#include <iostream>

inline void LuaStateTest(LuaState& lua)
{
	lua.SetValue("a", 1); //a = 1
	lua.Run("print(a)");

	int type; //lua type
	int n;
	
	type = lua.GetValue("a", &n);
	std::cout << n << std::endl << std::endl;

	lua.SetValue("a", nullptr); //a = nil
	lua.Run("print(a)");

	lua.SetValue("a", 1, 2); //a = {} a[1] = 2
	lua.Run("print(a[1])");

	type = lua.GetValue("a", 1, &n);
	std::cout << n << std::endl << std::endl;

	lua.GetValue("a", 1, LuaSetTo("a", 2)); //a[2] = a[1]
	lua.Run("print(a[2])");

	lua.SetValue("a", true, "abc", "def123"); //a[true] = {abc = 'def123'}
	lua.Run("print(a[true].abc)");

	const char* c;
	lua.GetValue("a", true, "abc", &c);
	std::cout << c << std::endl << std::endl;

	lua.SetValue("a", 123, LuaLoad("return e")); //a[123] = load("return e")

	lua.Run("print(a[123]())"); //e is nil

	//使用table t设置a[123]的方法ENV. Set table t to function ENV of a[123]
	lua.Run("t = {e = 1}");
	lua.SetValue("a", 123, LuaFEnv(), LuaGet("t"));

	lua.Run("print(a[123]())"); // e is 1

	lua.Run("a2 = {}");

	//a3 = {[a2] = 5}
	lua.SetValue("a3", LuaGet("a2"), 5);
	lua.Run("print(a3[a2])");

	//getmetatable(a3).__index = {z = w}
	lua.SetValue("a3", LuaMeta(), "__index", "z", "w");
	lua.Run("print(a3.z)");

	//a3[1] = 1 getmetatable(a3).__index[2] = 2
	lua.SetValue("a3", LuaSub(1, 1), LuaSub(LuaMeta(), "__index", 2, 2));
	lua.Run("print(a3[1], a3[2])");
}
