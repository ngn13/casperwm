#ifndef LOG_H
#define LOG_H
// #################################

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>

void info(const char*);
void error(const char* m);
void debug(const char* m);
void success(const char* m);
void out(const char*, const char*);

// #################################
#endif
