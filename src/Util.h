#ifndef __INCLUDE_UTIL_H__
#define __INCLUDE_UTIL_H__

/* Utility functions */
size_t UTIL_Format(char *buffer, size_t maxlength, char const *fmt, ...);
char *UTIL_Strdup(char const *str);

#endif