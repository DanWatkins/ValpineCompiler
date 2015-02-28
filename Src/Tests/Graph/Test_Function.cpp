#include "Tests.h"
#include <Graph/Function.h>

#define CLASS Test_Function
using namespace vc;

TEST_CLASS
{
};


TEST_CASE(Normal_WhatIS)
{
	//TODO we need to test Function::Type in this test
	#define tst(what) { auto f = graph::Function::make(what, graph::ScopeType::Root); \
						ASSERT_NOT_NULL(f); \
						ASSERT_EQ(f->type(), graph::Function::Type::Normal); }

	tst("void simple()")
	tst("int		 wacky	( ) ");
	tst(" bool	isCool(  )");
	tst("		float __compilerFunction ()");
	tst("		double whatIsPi(		)");

	tst(" int* pointer1()");
	tst(" *** void ** crazyPointerThatWontActuallyCompileButShouldLookLikeAFunction()");		//TODO should this look like function and flag as an error later, or just not even be considered a function?
	tst("const double& cosntRefPlease()");
	tst("	const void		* const *	const	*& superLongPointerRefThing()");

	tst("QString what()");
	tst("const __MyWeirdTyPe_________ *** _function_Name_Is_Here ( )");

	tst("void singleParameter(int p1)");
	tst("void defaultParameter(int count=12)");
	tst("float multipleDefaultParameters(int mag=10000, int size=147)");
	tst("BigBoolean multipleDefaultParameters(int mag=10000, int size=147)");
	tst("double someDefaultParameters(int a, float m, int size=50)");

	#undef tst
}


TEST_CASE(Constructor_WhatIs)
{
#define tst(what) { auto f = graph::Function::make(what, graph::ScopeType::ClassBlock); \
						ASSERT_NOT_NULL(f); \
						ASSERT_EQ(f->type(), graph::Function::Type::Constructor); }

	tst("Widget()");
	tst("Widget(const Widget &widget)");
	tst("my_type(const QString &name, int size)");

#undef tst
}


TEST_CASE(Destructor_WhatIs)
{
#define tst(what) { auto f = graph::Function::make(what, graph::ScopeType::ClassBlock); \
					ASSERT_NOT_NULL(f); \
					ASSERT_EQ(f->type(), graph::Function::Type::Destructor); }

	tst("~Widget()");
	tst("  ~ BigWidget ( )");
	tst("~ destructorWithParameter(int parameter)") //this should be considered a dtor, but should generate an error later

#undef tst
}


TEST_CASE(Normal_WhatIsNot)
{
#define tst(what) ASSERT_NULL(graph::Function::make(what, graph::ScopeType::Root))

	tst("123Type doesNotWork()");
	tst("NoGo 999Function		( )");
	tst("Don't useWeirdSybols()");
	tst("Ever !()");

	tst("float Not A Function()");
	tst("float functionDeclarationsDontExistInValpine() ;");

	tst("float thisIsImpossible(int a==3254, int m=  =	9832)");
	tst("return &(new bool);");

#undef tst
}


TEST_CASE(Destructor_WhatIsNot)
{
#define tst(what, scope) ASSERT_NULL(graph::Function::make(what, graph::ScopeType::scope))

	tst("~Widget()", Root);
	tst("~ Bigger_Widget( )", ExecutionBlock);

#undef tst
}