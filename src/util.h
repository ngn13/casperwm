#include <string.h>
#include <stdio.h>

#define startswith(a, b) strncmp(a, b, strlen(b)) == 0
void join(char* path1, char* path2, char* result);
bool stb(const char* s);
