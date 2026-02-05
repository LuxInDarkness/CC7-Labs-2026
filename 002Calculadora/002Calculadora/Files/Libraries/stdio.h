#ifndef STDIO_H
#define STDIO_H

void print(const char *s, ...);
void read(char *buffer, const char *s, ...);
int alpha2int(const char *s);
float alpha2float(const char *s);
char* int2alpha(int num, char *buffer);
char* float2alpha(float num, char *buffer);

#endif // STDIO_H
