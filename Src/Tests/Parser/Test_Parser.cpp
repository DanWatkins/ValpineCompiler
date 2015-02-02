#include "Tests.h"
#include <Parser/Parser.h>

#define CLASS Test_Parser
using namespace vc;

const QString gTestDir_Parser = gTestDir + "/Test_Parser/";

TEST_CLASS
{
};



TEST_CASE(HelloWorld)
{
	parser::Parser sp;
	sp.parseFile(gTestDir_Parser + "HelloWorld.val");

	using namespace graph;

	Block &root = sp.graph().block();
	ASSERT_EQ(2, root.statements().count());
	{
		Preprocessor *include = dynamic_cast<Preprocessor*>(root.statements().first());
		ASSERT_NOT_NULL(include);
		ASSERT_EQ("#include <iostream>", include->verbatim());
	
		Function *main = dynamic_cast<Function*>(root.statements().last());
		ASSERT_NOT_NULL(main);
		ASSERT_EQ("int main()", main->verbatim());
		ASSERT_EQ(2, main->block().statements().count());
		{
			ASSERT_EQ("std::cout << \"HelloWorld\" << std::endl;", main->block().statements().first()->verbatim());
			ASSERT_EQ("return 0;", main->block().statements().last()->verbatim());
		}
	}
}


TEST_CASE(Function)
{
	parser::Parser sp;
	sp.parseFile(gTestDir_Parser + "Function.val");

	using namespace graph;

	Block &root = sp.graph().block();
	ASSERT_EQ(3, root.statements().count());
	{
		QListIterator<Statement*> iter(root.statements());

		Preprocessor *include = dynamic_cast<Preprocessor*>(iter.next());
		ASSERT_NOT_NULL(include);
		ASSERT_EQ("#include <iostream>", include->verbatim());

		Function *main = dynamic_cast<Function*>(iter.next());
		ASSERT_NOT_NULL(main);
		ASSERT_EQ("int main()", main->verbatim());
		ASSERT_EQ(2, main->block().statements().count());
		{
			QListIterator<Statement*> iter(main->block().statements());
			ASSERT_EQ("foo();", iter.next()->verbatim());
			ASSERT_EQ("return 0;", iter.next()->verbatim());
		}

		Function *foo = dynamic_cast<Function*>(iter.next());
		ASSERT_NOT_NULL(foo);
		ASSERT_EQ("void foo()", foo->verbatim());
		ASSERT_EQ(1, foo->block().statements().count());
		{
			QListIterator<Statement*> iter(foo->block().statements());
			ASSERT_EQ("std::cout << \"Called foo\" << std::endl;", iter.next()->verbatim());
		}
	}
}


TEST_CASE(FunctionSignature)
{
	parser::Parser sp;
	sp.parseFile(gTestDir_Parser + "FunctionSignature.val");

	using namespace graph;

	Block &root = sp.graph().block();
	ASSERT_EQ(3, root.statements().count());
	{
		QListIterator<Statement*> iter(root.statements());

		Function *f1 = dynamic_cast<Function*>(iter.next());
		ASSERT_NOT_NULL(f1);
		EXPECT_EQ("access", f1->id());
		{
			graph::TypeExpression te = f1->returnType();
			EXPECT_EQ("int", te.baseType());
			EXPECT_EQ("*", te.postModifiers()[0]);
			{
				QVector<Parameter> prms = f1->parameters();
				ASSERT_EQ(2,prms.count());
				EXPECT_EQ("bool", prms[0].type.baseType());
				EXPECT_EQ("opened", prms[0].id);
				EXPECT_EQ("int", prms[1].type.baseType());
				EXPECT_EQ("count", prms[1].id);
			}
		}


		Function *f2 = dynamic_cast<Function*>(iter.next());
		ASSERT_NOT_NULL(f2);
		EXPECT_EQ("apply", f2->id());
		{
			graph::TypeExpression te = f2->returnType();
			EXPECT_EQ("void", te.baseType());
			{
				QVector<Parameter> prms = f2->parameters();
				ASSERT_EQ(1,prms.count());
				EXPECT_EQ("float", prms[0].type.baseType());
				EXPECT_EQ("amount", prms[0].id);
				EXPECT_EQ("14.0f", prms[0].defaultValue);

			}
		}


		Function *f3 = dynamic_cast<Function*>(iter.next());
		ASSERT_NOT_NULL(f3);
		EXPECT_EQ("longDistanceCommunication", f3->id());
		{
			graph::TypeExpression te = f3->returnType();
			EXPECT_EQ("bool", te.baseType());
			EXPECT_EQ("*", te.postModifiers()[0]);
			EXPECT_EQ("*", te.postModifiers()[1]);
			{
				QVector<Parameter> prms = f3->parameters();
				ASSERT_EQ(3,prms.count());
				EXPECT_EQ("Type1", prms[0].type.baseType());
				EXPECT_EQ("t1", prms[0].id);
				
				EXPECT_EQ("Type2", prms[1].type.baseType());
				EXPECT_EQ("t2", prms[1].id);
				
				EXPECT_EQ("float", prms[2].type.baseType());
				EXPECT_EQ("distance", prms[2].id);
				EXPECT_EQ("320.0f", prms[2].defaultValue);
			}
		}
	}
}