#pragma once

#include <string.h>
#include <stdio.h>
#include <stdbool.h>
#include <ctype.h>
#include <unistd.h>
#include <pwd.h>

#define startswith(a, b) strncmp(a, b, strlen(b)) == 0
void join(char*, char*, char*);
char* strlwr(char*);
bool stb(const char*);
char* get_homedir();
bool exists(char*);
