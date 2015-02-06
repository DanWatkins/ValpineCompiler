#include "Tests.h"
#include <Parser\Parser.h>
#include <Mocker\Mocker.h>

#define CLASS Test_Mocker
using namespace vc;

const QString gTestDir_Mocker = gTestDir + "/Test_Mocker/";

TEST_CLASS
{
};

TEST_CASE(HelloWorld)
{
	parser::Parser sp;
	sp.parseFile(gTestDir_Mocker + "HelloWorld.val");

	using namespace graph;
	using namespace mocker;

	Mocker mocker(const sp.graph());

}