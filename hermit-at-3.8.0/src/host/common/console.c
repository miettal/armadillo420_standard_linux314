#include <stdio.h>
#if defined(WIN32)
#include <windows.h>
#endif

#include "console.h"

int _console_message(const char *str){
	int len;
	len = fprintf(stderr, "%s",str);
	fflush(stdout);
	return len;
}

int _console_warning(const char *str){
	int len;
	len = fprintf(stderr, "%s",str);
	fflush(stderr);
	return len;
}

int _console_error(const char *str){
	int len;
	len = fprintf(stderr, "%s", str);
	fflush(stderr);
	return len;
}

#if defined(WIN32)
void _messagebox_using_sdk(const char *title, const char *str){
	MessageBox(NULL,str,title,0);
}
#endif

