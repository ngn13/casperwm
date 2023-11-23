#pragma once

#include <ini.h>
#include <pwd.h>
#include <unistd.h>
#include <string.h>
#include <stdbool.h>
#include <stdlib.h>

#include "log.h"
#include "util.h"

struct Binding {
  char* key;
  char* action;
  char* argument;
};

struct Config {
  bool debug;
  char* mod;
  bool border;
  int margin;
  int workspaces;
  int binding_count;
  struct Binding* bindings;
};

void init_cfg();
void free_cfg();
bool parse_cfg();

extern struct Config cfg;
