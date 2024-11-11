cmake_minimum_required(VERSION 3.9.4)

include(CheckIPOSupported)

project(hololang)


set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-error=c++20-compat")

function(convert_to_cstring_literal var value)
    string(REGEX REPLACE "([\\\"])" "\\\\\\1" escaped "${value}")
    string(REPLACE "\n" "\\n" escaped "${escaped}")
    set("${var}" "\"${escaped}\"" PARENT_SCOPE)
endfunction()

set(CMAKE_CXX_STANDARD 17)
set(REPL_SOURCE_PATH "${CMAKE_SOURCE_DIR}/src/prelude/repl.holo")


message(STATUS "REPL source in ${REPL_SOURCE_PATH}")
file(READ "${REPL_SOURCE_PATH}" REPL_SOURCE)
convert_to_cstring_literal(REPL_SOURCE "${REPL_SOURCE}")
message(STATUS "REPL code listing follows \n${REPL_SOURCE}")
add_definitions(-DFORTHSCRIPT_REPL_SOURCE=${REPL_SOURCE})


message(STATUS "Build type selected = ${CMAKE_BUILD_TYPE}")
if (CMAKE_BUILD_TYPE MATCHES Release)
    add_definitions(-DNDEBUG)
endif()

# Enabling warnings as errors
if (MSVC)
    add_compile_options(/W1)
else()
    add_compile_options(-Wall -Wextra -pedantic -Werror)
endif()

include_directories(include)

file(GLOB_RECURSE SOURCES src/*.cpp)

add_executable(hololang ${SOURCES})

# Enable LTO on release
if (CMAKE_BUILD_TYPE MATCHES Release)
    check_ipo_supported(RESULT supported OUTPUT error)
    if ( supported )
        message(STATUS "LTO builds are supported")
        set_property(TARGET hololang PROPERTY INTERPROCEDURAL_OPTIMIZATION TRUE)
    endif()
endif()

set_target_properties(hololang PROPERTIES RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_BINARY_DIR})
set_target_properties(hololang PROPERTIES RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_BINARY_DIR})
