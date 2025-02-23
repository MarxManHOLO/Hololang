cmake_minimum_required(VERSION 3.12)
project(Hololang VERSION 1.6.2 LANGUAGES C CXX)

option(HOLOLANG_BUILD_TESTING "Build unit test executable" OFF)
set(HOLOLANG_CMD_NAME "hololang" CACHE STRING
	"Specifies the command-line Hololang executable filename")

set(HOLOLANG_HEADERS
	Hololang/src/Hololang/Dictionary.h
        Hololang/src/Hololang/List.h
	Hololang/src/Hololang/HololangErrors.h
	Hololang/src/Hololang/HololangInterpreter.h
	Hololang/src/Hololang/HololangIntrinsics.h
	Hololang/src/Hololang/HololangKeywords.h
	Hololang/src/Hololang/HololangLexer.h
	Hololang/src/Hololang/HololangParser.h
	Hololang/src/Hololang/HololangTAC.h
	Hololang/src/Hololang/HololangTypes.h
	Hololang/src/Hololang/QA.h
	Hololang/src/Hololang/RefCountedStorage.h
	Hololang/src/Hololang/SimpleString.h
	Hololang/src/Hololang/SimpleVector.h
	Hololang/src/Hololang/SplitJoin.h
	Hololang/src/Hololang/UnicodeUtil.h
	Hololang/src/Hololang/UnitTest.h
	
)

set(HOLOLANGCMD_HEADERS
	Hololang/src/DateTimeUtils.h
	Hololang/src/Key.h
	Hololang/src/OstreamSupport.h
	Hololang/src/ShellExec.h
	Hololang/src/ShellIntrinsics.h
	Hololang/src/editline/editline.h
	Hololang/src/editline/unix.h
	Hololang/src/whereami/whereami.h
)

add_library(Hololang
	Hololang/src/Hololang/Dictionary.cpp
	Hololang/src/Hololang/List.cpp
	Hololang/src/Hololang/HololangInterpreter.cpp
	Hololang/src/Hololang/HololangIntrinsics.cpp
	Hololang/src/Hololang/HololangKeywords.cpp
	Hololang/src/Hololang/HololangLexer.cpp
	Hololang/src/Hololang/HololangParser.cpp
	Hololang/src/Hololang/HololangTAC.cpp
	Hololang/src/Hololang/HololangTypes.cpp
	Hololang/src/Hololang/QA.cpp
	Hololang/src/Hololang/SimpleString.cpp
	Hololang/src/Hololang/SimpleVector.cpp
	Hololang/src/Hololang/SplitJoin.cpp
	Hololang/src/Hololang/UnicodeUtil.cpp
	Hololang/src/Hololang/UnitTest.cpp
	${HOLOLANG_HEADERS}
)

target_include_directories(Hololang PUBLIC Hololang/src/Hololang)

if(NOT WIN32)
	set(EDITLINE_SRC
		Hololang/src/editline/complete.c
		Hololang/src/editline/editline.c
		Hololang/src/editline/sysunix.c
	)
endif()

add_executable(Hololangcmd
	Hololang/src/main.cpp
	Hololang/src/DateTimeUtils.cpp
	Hololang/src/Key.cpp
	Hololang/src/OstreamSupport.cpp
	Hololang/src/ShellIntrinsics.cpp
	Hololang/src/ShellExec.cpp
	Hololang/src/whereami/whereami.c
	${EDITLINE_SRC}
	${HOLOLANGCMD_HEADERS}
)
target_include_directories(Hololangcmd PRIVATE Hololang/src/editline)
target_link_libraries(Hololangcmd PRIVATE Hololang)

set_target_properties(Hololang Hololangcmd PROPERTIES
	CXX_STANDARD 14
	CXX_STANDARD_REQUIRED ON)
set_target_properties(Hololangcmd PROPERTIES OUTPUT_NAME ${HOLOLANG_CMD_NAME})

if(HOLOLANG_BUILD_TESTING)
	enable_testing()
	add_custom_target(TestSuite SOURCES TestSuite.txt)
	add_executable(tests-cpp Hololang/src/Hololang/UnitTest.cpp)
	target_compile_definitions(tests-cpp PRIVATE UNIT_TEST_MAIN)
	target_link_libraries(tests-cpp PRIVATE Hololang)
	add_test(NAME Hololang.cpp.UnitTests COMMAND tests-cpp)
	add_test(NAME Hololang.cpp.Integration COMMAND Hololangcmd --itest ${CMAKE_SOURCE_DIR}/TestSuite.txt)
	set_tests_properties(Hololang.cpp.UnitTests Hololang.cpp.Integration PROPERTIES FAIL_REGULAR_EXPRESSION "FAIL|Error")
endif()

install(TARGETS Hololang Hololangcmd)
