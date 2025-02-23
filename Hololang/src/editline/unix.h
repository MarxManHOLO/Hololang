#ifndef EDITLINE_UNIX_H_
#define EDITLINE_UNIX_H_

#define CRLF		"\r\n"
#define FORWARD		STATIC

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#include <dirent.h>
typedef struct dirent	DIRENTRY;

#endif 
