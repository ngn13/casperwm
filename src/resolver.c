#include <string.h> 
#include <X11/Xlib.h>

#include "resolver.h"
#include "util.h"

struct ModMapping mod_mappings[] = {
  {.mod = "mod1", .key = Mod1Mask},
  {.mod = "mod2", .key = Mod2Mask},
  {.mod = "mod3", .key = Mod3Mask},
  {.mod = "mod4", .key = Mod4Mask},
  {.mod = "mod5", .key = Mod5Mask},
}; 

int mod_resolver(char* mod) {
  char* mod_lwr = strlwr(mod); 
  for(int i = 0; i < sizeof(mod_mappings)/sizeof(struct ModMapping); i++){
    if(strcmp(mod_mappings[i].mod, mod_lwr)==0) {
      return mod_mappings[i].key;
    }
  }

  return AnyModifier;
}
