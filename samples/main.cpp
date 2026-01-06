#include "pch.h"
#include "../LuaWrapper/LuaUtility.h"
#include <iostream>

void OnLuaError(const char* s)
{
	std::cout << s << std::endl;
}

int f1(int a, int b)
{
	return a + b;
}

std::tuple<int, float, bool, const char*, void*> f2(int a, float b, bool c, const char* d, void* e)
{
	return { a, b, c, d, e };
}
Lua_global_add_cfunc(f2) //添加到全局环境

//使用LuaReturn作为lua返回值，方法返回类型必须为void
//using LuaReturn for return value for lua, return type must be void
void f3(LuaReturn& ret, int a, float b, bool c, const char* d, void* e)
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
int f4(LuaIdx t, int a, int b)
{
	int n;
	t.GetValue(a, &n);
	std::cout  << "get t[a]: " << n << std::endl;
	//t[a] = b
	t.SetValue(a, b);
	return a + b;
}
Lua_global_add_cfunc(f4)


class ClassA //wrapped ClassA
{
public:
	ClassA()
	{
		std::cout << "ClassA constructed " << std::endl;
	}
	~ClassA()
	{
		std::cout << "ClassA destructed." << std::endl << std::endl;
	}
	void f1()
	{
		std::cout << "ClassA f1 called." << std::endl;
	}
	static void static_f1()
	{
		std::cout << "ClassA static_f1 called." << std::endl;
	}
	Lua_wrap_cpp_class(ClassA, Lua_ctor_void, Lua_mf(f1), Lua_mf(static_f1)) //wrap the class
};

class ClassA2 //none wrapped ClassA2
{
public:
	std::tuple<const char*, int> f2(int n)
	{
		return { "ClassA2 f2 called. Param:", n };
	}
};

class ClassB : public ClassA, public ClassA2 //wrapped ClassB
{
public:
	ClassB(int n)
	{
		std::cout << "ClassB constructed. Param: " << n << std::endl;
	}
	~ClassB()
	{
		std::cout << "ClassB destructed." << std::endl;
	}
	Lua_wrap_cpp_class_derived(ClassA, ClassB, Lua_ctor(int), Lua_mf(f2)) //f2 is from ClassA2
};
Lua_global_add_cpp_class(ClassB) //添加到全局环境

int main()
{
	LuaState::SetErrorFunc(OnLuaError);

	LuaState lua;
	
	lua.SetValue("a", 1); //a = 1
	lua.Run("print(a)");

	int type; //lua type
	int n;
	type = lua.GetValue("a", &n);
	std::cout << n << std::endl << std::endl;

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

	//set ENV of a[123] width table t 
	lua.Run("t = {e = 1}");
	lua.SetValue("a", 123, LuaFEnv(), LuaGet("t"));
	
	lua.Run("print(a[123]())"); // e is 1

	//将包装后的f1 C++方法赋给f1 assign f1 with wrapped f1 C++ method
	lua.SetValue("f1", Lua_cf(f1));

	//在lua中调用f1 call f1 in lua
	lua.Run("print(f1(1, 1))");		

	//在C++调用f1, n为返回值 call f1 in C++， n for return value
	lua.GetValue("f1", LuaCall(1, 2), &n); 
	std::cout << n << std::endl << std::endl;

	//注册全局的lua方法 register global functions and classes to lua
	LuaRegGlobalReflected(lua.Lua());
	
	//将n作为userdata赋给n assign n width n as userdata
	lua.SetValue("n", &n);

	//call f2 in lua
	lua.Run("print(f2(1, 2.5, true, 'abcde', n))"); 

	//call f3 in lua
	lua.Run("print(f3(1, 2.5, true, 'abcde', n))");

	//call f4 in lua
	lua.Run("a[1] = 1 f4(a, 1, 5) print(a[1])");
	
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

	//a3 = {[a2] = 5}
	lua.SetValue("a3", LuaGet("a2"), 5);
	lua.Run("print(a3[a2])");

	//getmetatable(a3).__index = {z = w}
	lua.SetValue("a3", LuaMeta(), "__index", "z", "w");
	lua.Run("print(a3.z)");

	//ClassA is not global reflected, so register ClassA
	LuaRegisterCppClass<ClassA>(lua.Lua());

	//instantiate c1 from ClassA and call f1, static_f1
	lua.Run("c1 = ClassA() c1:f1() c1.static_f1()");

	//collect c1
	lua.Run("c1 = nil collectgarbage('collect')");

	//ClassB is global reflected, instantiate c1 from ClassB and call f1, static_f1 and f2
	lua.Run("c2 = ClassB(1) c2:f1() c2.static_f1() print(c2:f2(1))");

	//collect c2
	lua.Run("c2 = nil collectgarbage('collect')");

	getchar();
}
