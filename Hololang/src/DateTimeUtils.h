

#ifndef DateTimeUtils_hpp
#define DateTimeUtils_hpp

#include <stdio.h>
#include <time.h>
#include "Hololang/SimpleString.h"

namespace Hololang {

String FormatDate(time_t dateTime, String formatSpec);

inline String FormatDate(time_t dateTime) {
	return FormatDate(dateTime, "yyyy-MM-dd HH:mm:ss");
}

time_t ParseDate(const String dateStr);

}

#endif 
