#ifndef MSQA_H
#define MSQA_H

#include <ciso646>  

namespace Hololang {
	
	extern void _Error(const char *msg, const char *filename, const int linenum);
	inline void _ErrorIf(int condition, const char *msg, const char *filename, int linenum)
		{ if (condition) _Error(msg, filename, linenum); }

}

#define Error(msg) Hololang::_Error(msg, __FILE__, __LINE__)
#define ErrorIf(condition) Hololang::_ErrorIf(condition, "Error: " # condition, __FILE__, __LINE__)
#ifdef Assert
#undef Assert
#endif
#define Assert(condition) Hololang::_ErrorIf(!(condition), "Assert failed: " # condition, __FILE__, __LINE__)

#endif 
