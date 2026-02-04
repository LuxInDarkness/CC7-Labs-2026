#ifndef STDIO_H
#define STDIO_H

#include <stddef.h>

void print(const char *s, ...);
int uart_atoi(const char *s);
char* uart_itoa(int num, char *buffer);

#endif // STDIO_H
