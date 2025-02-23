

#ifndef KEY_MODULE_H
#define KEY_MODULE_H

#include <stdio.h>
#include <wchar.h>
#include "./Hololang/HololangTypes.h"
#include "./Hololang/SimpleVector.h"

namespace Hololang {



struct InputBufferEntry {
	wchar_t c;         
	wchar_t scanCode;  
};


extern SimpleVector<struct InputBufferEntry> inputBuffer;


ValueDict& KeyDefaultScanMap();


void KeyOptimizeScanMap(ValueDict& scanMap);


Value KeyAvailable();


Value KeyGet(ValueDict& scanMap);


void KeyPutCodepoint(long codepoint, bool inFront=false);


void KeyPutString(String s, bool inFront=false);


void KeyClear();


bool KeyGetEcho();


void KeySetEcho(bool on);


}

#endif 
