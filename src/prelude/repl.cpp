#include <prelude/repl.hpp>

std::u32string getPreludeREPLSource() {

#ifndef FORTHSCRIPT_REPL_SOURCE
    return U"\"usage: forthscript <path/to/file>\" writeln 1 exit";
#else
    std::u32string toUTF32(const std::string &src);
    return toUTF32(FORTHSCRIPT_REPL_SOURCE);
#endif
}
