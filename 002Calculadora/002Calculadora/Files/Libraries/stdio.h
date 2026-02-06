#ifndef STDIO_H
#define STDIO_H

#include <stdarg.h>

void print(const char *s, ...);
void vprint(const char *s, va_list args);
void read(char *buffer, int buffer_size, const char *s, ...);
int alpha2int(const char *s);
float alpha2float(const char *s);
char* int2alpha(int num, char *buffer);
char* float2alpha(float num, char *buffer);

#endif // STDIO_H
