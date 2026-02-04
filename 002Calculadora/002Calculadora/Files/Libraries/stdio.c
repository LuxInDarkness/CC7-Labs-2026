#include <stdarg.h>
#include "uart_io.h"

#define INT_TO_STRING_BUFFER_SIZE 16
#define PRINT_BUFFER_SIZE 256

// Simple function to convert string to integer
int uart_atoi(const char *s) {
    int num = 0;
    int sign = 1;
    int i = 0;

    // Handle optional sign
    if (s[i] == '-') {
        sign = -1;
        i++;
    }

    for (; s[i] >= '0' && s[i] <= '9'; i++) {
        num = num * 10 + (s[i] - '0');
    }

    return sign * num;
}

// Function to convert integer to string
char * uart_itoa(int num, char *buffer) {
    int i = 0;
    int is_negative = 0;

    if (num == 0) {
        buffer[i++] = '0';
        buffer[i] = '\0';
        return buffer;
    }

    if (num < 0) {
        is_negative = 1;
        num = -num;
    }

    while (num > 0 && i < 14) { // Reserve space for sign and null terminator
        buffer[i++] = '0' + (num % 10);
        num /= 10;
    }

    if (is_negative) {
        buffer[i++] = '-';
    }

    buffer[i] = '\0';

    // Reverse the string
    int start = 0, end = i - 1;
    char temp;
    while (start < end) {
        temp = buffer[start];
        buffer[start] = buffer[end];
        buffer[end] = temp;
        start++;
        end--;
    }

    return buffer;
}

void print(const char *s, ...) {
    int mod_flag = 0;
    char final[PRINT_BUFFER_SIZE];
    char *final_ptr = final;
    char *final_end = final + PRINT_BUFFER_SIZE - 1;

    va_list args;
    va_start(args, s);

    while (*s && final_ptr < final_end) {
        if (*s == '%') {
            mod_flag = 1;
        } else if (mod_flag) {
            if (*s == 's') {
                char *curr_str = va_arg(args, char*);
                while (*curr_str && final_ptr < final_end) {
                    *final_ptr++ = *curr_str++;
                }
            } else if (*s == 'd') {
                char buffer[INT_TO_STRING_BUFFER_SIZE];
                int curr_int = va_arg(args, int);
                char *curr_str = uart_itoa(curr_int, buffer);
                while (*curr_str && final_ptr < final_end) {
                    *final_ptr++ = *curr_str++;
                }
            } else {
                if (final_ptr + 1 < final_end) {  // ✅ Check space for 2 chars
                    *final_ptr++ = '%';
                    *final_ptr++ = *s;
                }
            }
            mod_flag = 0;
        } else {
            *final_ptr++ = *s;
        }
        s++;
    }

    *final_ptr = '\0';
    va_end(args);
    uart_puts(final);
}