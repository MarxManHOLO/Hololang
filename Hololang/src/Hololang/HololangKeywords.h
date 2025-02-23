#ifndef HOLOLANGKEYWORDS_H
#define HOLOLANGKEYWORDS_H

#include "SimpleString.h"

namespace Hololang {

	class Keywords {
	public:
		static const String all[];
		
		static const int count;
		
		static bool IsKeyword(String text) {
			for (int i=0; i<count; i++) if (all[i] == text) return true;
			return false;
		}
	};
		
}

#endif
