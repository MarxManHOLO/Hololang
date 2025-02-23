#include "QA.h"
#include <stdio.h>

namespace Hololang {
	
	void _Error(const char *msg, const char *filename, int linenum)
	{
		#if WINDOWS
			OutputDebugString(String(msg) + " at " + filename + ":" + ultoa(linenum));
		#else
			fprintf( stderr, "%s at %s:%d\n", msg, filename, linenum);
			fflush( stderr );
		#endif
	}

}
