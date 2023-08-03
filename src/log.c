#include "log.h"
#include "config.h"
#include <stdlib.h>
#include <stdbool.h>

void out(const char* tag, const char* m){
   time_t now;
   time(&now);
   struct tm * p = localtime(&now);

   char s[1000];
   strftime(s, sizeof s, "%H:%M:%S", p);

   printf("[%s] [%s]: %s\n", s, tag, m);
}

void info(const char* m){
  out("INFO   ", m);
}

void success(const char* m){
  out("SUCCESS", m);
}

void error(const char* m){
  out("ERROR  ", m);
  //exit(EXIT_FAILURE);
}

void debug(const char* m){
  if(DEBUG){
    out("DEBUG  ", m);
  }
}
