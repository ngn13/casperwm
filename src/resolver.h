#pragma once

#include <string.h> 
#include <X11/Xlib.h>

struct ModMapping {
  char* mod;
  int key;
};

int mod_resolver(char* mod);
