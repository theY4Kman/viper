#include "Util.h"
#include "STL.h"

size_t UTIL_Format(char *buffer, size_t maxlength, char const *fmt, ...) {
	va_list ap;
	va_start(ap, fmt);
	size_t len = vsnprintf(buffer, maxlength, fmt, ap);
	va_end(ap);

	if(len >= maxlength) {
		buffer[maxlength - 1] = '\0';
		return (maxlength - 1);
	}
	
	return len;
}

char *UTIL_Strdup(char const *str) {
	char *ptr = new char[strlen(str)+1];
	strcpy(ptr, str);
	return ptr;
}