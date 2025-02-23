#ifndef UNITTEST_H
#define UNITTEST_H

#include "QA.h"

namespace Hololang {
	

	class UnitTest
	{
	  public:
		UnitTest(const char* testName) : name(testName), mNext(0) {}
		
		
		virtual void Run()=0;
		
		
		virtual void SetUp() {}
		virtual void TearDown() {}

		
		static void RegisterTestCase(UnitTest *test);
		
		
		static void RunAllTests();
		static void DeleteAllTests();

		const char *name;

	  protected:
		

	  private:
		UnitTest *mNext;
		
		static UnitTest *cTests;
	};

	class UnitTestRegistrar
	{
	  public:
		UnitTestRegistrar(UnitTest *test) { UnitTest::RegisterTestCase(test); }
	};

}


#define RegisterUnitTest(testclass) static testclass _inst##testclass; \
									UnitTestRegistrar _reg##testclass(&_inst##testclass)

#endif
