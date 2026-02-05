#include <stdarg.h>
#include "uart_io.h"
#include "stdio.h"

#define INT_TO_STRING_BUFFER_SIZE 16
#define FLOAT_TO_STRING_BUFFER_SIZE 32
#define PRINT_BUFFER_SIZE 256

// Simple function to convert string to integer
int alpha2int(const char *s) {
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

float alpha2float(const char *s) {
    float num = 0;
    float sign = 1;
    int i = 0;
    int f = 10;

    // Handle optional sign
    if (s[i] == '-') {
        sign = -1;
        i++;
    }

    for (; s[i] >= '0' && s[i] <= '9'; i++) {
        num = num * 10 + (s[i] - '0');
    }

    if (s[i] != '.') return sign * num;

    i++;
    for (; s[i] >= '0' && s[i] <= '9'; i++) {
        num = num + (s[i] - '0') / f;
        f *= 10;
    }

    return sign * num;
}

// Function to convert integer to string
char * int2alpha(int num, char *buffer) {
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

char * float2alpha(float num, char *buffer) {
    int i = 0;
    int is_negative = 0, multi = 10;
    int f_value, d_value;
    float tolerance = 0.0000001;
    char f_string[INT_TO_STRING_BUFFER_SIZE], d_string[INT_TO_STRING_BUFFER_SIZE];
    char *full, *decimal;
    float temp;

    if (num == 0) {
        buffer[i++] = '0';
        buffer[i] = '\0';
        return buffer;
    }

    if (num < 0) {
        is_negative = 1;
        num = -num;
    }

    f_value = (int)num;
    full = int2alpha(f_value, f_string);

    temp = num - (float)f_value;
    while (temp - (float)(int)temp > tolerance) {
        temp *= (float)multi;
        multi *= 10;
    }

    d_value = (int)temp;
    decimal = int2alpha(d_value, d_string);

    if (is_negative) {
        buffer[i++] = '-';
    }

    while (*full) {
        buffer[i++] = *full++;
    }
    buffer[i++] = '.';
    while (*decimal) {
        buffer[i++] = *decimal++;
    }

    buffer[i] = '\0';

    return buffer;
}

void vprint(const char *s, va_list args) {
    int mod_flag = 0;
    char final[PRINT_BUFFER_SIZE];
    char *final_ptr = final;
    char *final_end = final + PRINT_BUFFER_SIZE - 1;

    while (*s && final_ptr < final_end) {
        if (*s == '%') {
            mod_flag = 1;
        } else if (mod_flag) {
            if (*s == 's') {
                char *curr_str = va_arg(args, char*);
                if (curr_str) {
                    while (*curr_str && final_ptr < final_end) {
                        *final_ptr++ = *curr_str++;
                    }
                }
            } else if (*s == 'd') {
                char buffer[INT_TO_STRING_BUFFER_SIZE];
                int curr_int = va_arg(args, int);
                char *curr_str = int2alpha(curr_int, buffer);
                while (*curr_str && final_ptr < final_end) {
                    *final_ptr++ = *curr_str++;
                }
            } else if (*s == 'f') {
                char buffer[FLOAT_TO_STRING_BUFFER_SIZE];
                float curr_float = (float)va_arg(args, double);
                char *curr_str = float2alpha(curr_float, buffer);
                while (*curr_str && final_ptr < final_end) {
                    *final_ptr++ = *curr_str++;
                }
            } else {
                if (final_ptr + 1 < final_end) {
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
    uart_puts(final);
}

void print(const char *s, ...) {
    va_list args;
    va_start(args, s);
    vprint(s, args);
    va_end(args);
}

void read(char *buffer, const char *s, ...) {
    va_list args;
    va_start(args, s);

    vprint(s, args);

    va_end(args);

    uart_gets_input(buffer, PRINT_BUFFER_SIZE);
}