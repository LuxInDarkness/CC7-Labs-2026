#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "log_parser.h"

// extracts the ip address, e.g. "192.168.1.1"
void parse_ip(char *line, char *out, size_t out_size) {
    const char *end = strchr(line, ' ');
    if (!end) return;

    size_t len = end - line;
    if (len >= out_size) len = out_size - 1;
    strncpy(out, line, len);
    out[len] = '\0';
}

// extracts the web page path, e.g. "/index.html"
void parse_url(char *line, char *out, size_t out_size) {
    const char *start = strchr(line, '"');
    if (!start) return;

    const char *url_start = strchr(start + 1, '/');
    if (!url_start) return;

    const char *end = strchr(url_start, '"');   // path ends at closing quote
    if (!end) return;

    size_t len = end - url_start;
    if (len >= out_size) len = out_size - 1;
    strncpy(out, url_start, len);
    out[len] = '\0';
}

// extracts the return code, e.g. "200"
int32_t parse_error_code(char *line) {
    const char *p = strrchr(line, ' ');
    if (!p) return -1;

    int32_t code = (int32_t)atoi(p + 1);
    return code;
}