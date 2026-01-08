#pragma once
#include "../LuaCWrapper/LuaGlobalReflect.h"
#include <iostream>

class ClassA //wrapped ClassA
{
public:
	ClassA()
	{
		std::cout << "ClassA constructed " << std::endl;
	}
	~ClassA()
	{
		std::cout << "ClassA destructed." << std::endl;
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
		m_num = n;
	}
	~ClassB()
	{
		std::cout << "ClassB destructed." << std::endl;
	}
	int f3() { return m_num; }
	Lua_wrap_cpp_class_derived(ClassA, ClassB, Lua_ctor(int), Lua_mf(f2), //f2 is from ClassA2
		Lua_mf(f3));
	int m_num;
};
Lua_global_add_cpp_class(ClassB) //添加到全局环境

//LuacObjNew类型返回的对象由lua回收 object returned by the type of LuacObjNew will be collected by lua
LuacObjNew<ClassB> f5(LuacObj<ClassA> a, LuacObj<ClassB> b, int num)
{
	a->f1();
	b->f2(1);
	return new ClassB(num);
}
Lua_global_add_cfunc(f5)

class ClassT
{
public:
	ClassT(int n) : m_num(n) {}

	int GetNum() { return m_num; }

	int m_num;
};


void ClassTest(LuaState& lua)
{
	//ClassA is not global reflected, so register ClassA
	LuaRegisterCppClass<ClassA>(lua.Lua());

	//instantiate c1 from ClassA and call f1, static_f1
	lua.Run("c1 = ClassA() c1:f1() c1.static_f1()");

	//collect c1
	lua.Run("c1 = nil collectgarbage('collect')");

	//ClassB is global reflected, instantiate c1 from ClassB and call f1, static_f1 and f2
	lua.Run("c2 = ClassB(1) c2:f1() c2.static_f1() print(c2:f2(1))");

	lua.Run("c3 = f5(c2, c2, 5) print(c3:f3())");

	//collect c2
	lua.Run("c2 = nil collectgarbage('collect')");

	//collect c3
	lua.Run("c3 = nil collectgarbage('collect')");

	ClassA a;
	lua.SetValue("a", Lua_set_cobj(&a)); //a由C++回收 a will be collected by C++
	lua.Run("print(a:f1())");
}