#include "LuaStateTest.h"
#include "FunctionTest.h"
#include "ClassTest.h"
#include "LStringTest.h"
#include <conio.h>

void OnLuaError(const char* s)
{
	std::cout << s << std::endl;
}

int main()
{
	LuaState::SetErrorFunc(OnLuaError);

	LuaState lua; //LuaState初始化方法1 LuaState initialize method 1

	int top = lua.GetTop();
	
	//lua_State* L = luaL_newstate();
	//luaL_openlibs(L);
	//LuaState lua(L); //LuaState初始化方法2. LuaState initialize method 2

	LuaRegGlobalReflected(lua.Lua()); //注册全局的lua方法. Register global functions and classes to lua

	std::cout << "-----LuaStateTest-----" << std::endl;
	LuaStateTest(lua);

	std::cout << std::endl << "-----FunctionTest-----" << std::endl;
	FunctionTest(lua);

	std::cout << std::endl << "-----ClassTest-----" << std::endl;
	ClassTest(lua);

	std::cout << std::endl << "-----LStringTest-----" << std::endl;
	LStringTest(lua);

	Assert(lua.GetTop() == top); //lua栈须保持平衡. Lua stack should maintain balance

	//lua_close(L); //初始化方法2需要删除lua_State*. Initialize method 2 need to delete lua_State* manually
	//delete = L;

	_getch();
}
