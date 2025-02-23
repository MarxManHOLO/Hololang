#include "UnitTest.h"
#include "QA.h"
#include <stdio.h>
#include <iostream>
#include "SimpleString.h"

namespace Hololang {
	
	UnitTest *UnitTest::cTests = 0;


	void UnitTest::RegisterTestCase(UnitTest *test)
	{
		if (cTests) {
			test->mNext = cTests;
		}
		cTests = test;
	}

	
	void UnitTest::RunAllTests()
	{
		for (UnitTest *test = cTests; test; test = test->mNext) {
//			std::cout << "Running " << test->name << std::endl;
			test->SetUp();
			test->Run();
			test->TearDown();
//			std::cout << "StringStorage instances left: " << StringStorage::instanceCount << std::endl;
//			std::cout << "total RefCountedStorage instances left: " << RefCountedStorage::instanceCount << std::endl;
		}
	}

	
	void UnitTest::DeleteAllTests()
	{
		UnitTest *next;
		for (UnitTest *test = cTests; test; test = next) {
			next = test->mNext;
			
			//delete test;
		}
		cTests = 0;
	}

	//--------------------------------------------------------------------------------
	
	class TestCaseItself : public UnitTest
	{
	public:
		TestCaseItself() : UnitTest("TestCaseItself") {}
		virtual void Run();
	};

	void TestCaseItself::Run()
	{
		ErrorIf(1==0);
		Assert(2+2==4);
		// NOTE: can't really test the actual error reporting, because if
		// it works it'd look like an error!
	}

	RegisterUnitTest(TestCaseItself);

}

#ifdef UNIT_TEST_MAIN
int main(int, const char*[]) {
	MiniScript::UnitTest::RunAllTests();
}
#endif

