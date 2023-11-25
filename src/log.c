#include <stdarg.h>
#include <stdio.h>
#include <time.h>

#include "config.h"

void print(const char* tag, const char* msg, ...){
  va_list args;
  va_start(args, msg);

  time_t now;
  time(&now);
  struct tm * p = localtime(&now);

  char s[1000];
  strftime(s, sizeof s, "%H:%M:%S", p);

  printf("[%s] [%s]: ", s, tag);
  vprintf(msg, args);
  printf("\n");
  
  va_end(args);
}

void debug(const char* msg, ...) {
  if(!cfg.debug) {
    return;
  }

  va_list args;
  va_start(args, msg);

  time_t now;
  time(&now);
  struct tm * p = localtime(&now);

  char s[1000];
  strftime(s, sizeof s, "%H:%M:%S", p);

  printf("[%s] [DEBUG]: ", s);
  vprintf(msg, args);
  printf("\n");
  
  va_end(args);
}
