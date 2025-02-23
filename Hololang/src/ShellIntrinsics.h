#ifndef SHELLINTRINSICS_H
#define SHELLINTRINSICS_H

#include "Hololang/HololangTypes.h"
#if _WIN32
	#define useEditline 0
#else
	#include "editline.h"
	#define useEditline 1
#endif

extern bool exitASAP;
extern int exitResult;

extern Hololang::ValueList shellArgs;

void AddPathEnvVars();
void AddScriptPathVar(const char* scriptPartialPath);
void AddShellIntrinsics();

#endif 
