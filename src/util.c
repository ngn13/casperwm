#include <string.h>
#include <stdio.h>
#include <stdbool.h>

#include "util.h"

// joins 2 paths together
void join(char* path1, char* path2, char* result) {
  char path1_end = path1[strlen(path1)-1];
  char path2_start = path2[0];

  if(path1_end == '/' || path2_start == '/') {
    sprintf(result, "%s%s", path1, path2);
    return;
  }

  sprintf(result, "%s/%s", path1, path2);
}

// string to bool 
bool stb(const char* s) {
  if(strcmp(s, "true") == 0) {
    return true;
  }

  return false;
}
