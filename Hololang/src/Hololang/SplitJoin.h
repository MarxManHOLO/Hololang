#ifndef SPLITJOIN_H
#define SPLITJOIN_H

#include "List.h"
#include "SimpleString.h"

namespace Hololang {

	typedef List<String> StringList;

	StringList Split(const String& s, const String& delimiter=" ", int maxSplits=-1);
	StringList Split(const String& s, char delimiter, int maxSplits=-1);

	String Join(const String& delimiter, const StringList& parts);
	String Join(char delimiter, const String& parts);

}

#endif
