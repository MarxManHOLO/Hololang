#include "SimpleString.h"
#include "HololangKeywords.h"

namespace Hololang {
	
	const String Keywords::all[] = {
		"break",
		"continue",
		"else",
		"end",
		"for",
		"function",
		"if",
		"in",
		"isa",
		"new",
		"null",
		"then",
		"repeat",
		"return",
		"while",
		"and",
		"or",
		"not",
		"true",
		"false"
	};
	
	const int Keywords::count = sizeof(all) / sizeof(String);
	
}
