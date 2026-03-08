#ifndef log_parser_h
#define log_parser_h

#include <stdint.h>

int32_t parse_error_code(char *line);
void parse_ip(char *line, char *out, size_t out_size);
void parse_url(char *line, char *out, size_t out_size);

#endif /* log_parser_h */