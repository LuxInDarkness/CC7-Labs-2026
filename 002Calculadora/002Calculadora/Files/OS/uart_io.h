#ifndef UART_IO_H
#define UART_IO_H

void uart_putc(char c);
char uart_getc();
void uart_puts(const char *s);
void uart_gets_input(char *buffer, int max_length);

#endif // UART_IO_H
