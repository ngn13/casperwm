#include <ini.h>
#include <unistd.h>
#include <string.h>
#include <stdbool.h>
#include <stdlib.h>

#include "config.h"
#include "log.h"
#include "util.h"

struct Config cfg = {
  .debug = false,
  .bar = "",
  .border_active = "",
  .border_inactive = "",
  .border_width = 0,
  .float_step = 10,
};
char* last_section = "";

// handles the parsing 
static int parse_handler(void* idk, const char* section, const char* key, const char* value) {
  #define MATCH(s, n) strcmp(section, s) == 0 && strcmp(key, n) == 0
  #define MATCHKEY(k) strcmp(key, k) == 0

  if(MATCH("general", "debug")) {
    cfg.debug = stb(value); 
    return 1;
  }
  
  if(MATCH("general", "mod")) {
    cfg.mod = strdup(value);
    return 1;
  }

  if(MATCH("general", "border_active")) {
    cfg.border_active = strdup(value);
    return 1;
  }
 
  if(MATCH("general", "border_inactive")) {
    cfg.border_inactive = strdup(value);
    return 1;
  }
 
  if(MATCH("general", "border_width")) {
    cfg.border_width = atoi(value);
    return 1;
  }

  if(MATCH("general", "margin")) {
    cfg.margin = atoi(value);
    return 1;
  }

  if(MATCH("general", "workspaces")) {
    cfg.workspaces = atoi(value);
    return 1;
  }

  if(MATCH("general", "float_step")) {
    cfg.float_step = atoi(value);
    return 1;
  }

  if(MATCH("general", "bar")) {
    cfg.bar = strdup(value);
    return 1;
  }

  if(startswith(section, "bind") == false) {
    print("CONFIG", "Invalid section: %s", section);
    return 0;
  }

  if(strlen(last_section)!=0 && strcmp(last_section, section)!=0) {
    last_section = strdup(section);
    cfg.binding_count++;
    cfg.bindings = realloc(cfg.bindings, sizeof(struct Binding)*cfg.binding_count);
    cfg.bindings[cfg.binding_count-1].has_mod = false;
  } 

  if(strlen(last_section) == 0) {
    last_section = strdup(section);
  }

  if(MATCHKEY("key")) {
    cfg.bindings[cfg.binding_count-1].key = strdup(value);
    return 1;
  }

  if(MATCHKEY("action")) {
    cfg.bindings[cfg.binding_count-1].action = strdup(value);
    return 1;
  }

  if(MATCHKEY("mod")) {
    cfg.bindings[cfg.binding_count-1].mod = strdup(value);
    cfg.bindings[cfg.binding_count-1].has_mod = true;
    return 1;
  }

  if(MATCHKEY("argument")) {
    cfg.bindings[cfg.binding_count-1].argument = strdup(value);
    return 1;
  }

  return 0;
}

void init_cfg() {
  cfg.binding_count = 1;
  cfg.bindings = malloc(sizeof(struct Binding));
}

void free_cfg() {
  free(cfg.bindings);
}

// parses the ini config file
bool parse_cfg() {
  char* homedir = get_homedir();

  char cfg_path[strlen(homedir)+200];
  join(homedir, ".config/casperwm/config", cfg_path);

  if(ini_parse(cfg_path, parse_handler, &cfg) < 0) {
    print("CONFIG", "Cannot load the config: %s", cfg_path);
    return false;
  }

  print("CONFIG", "Loaded config: %s", cfg_path);
  return true;
}
