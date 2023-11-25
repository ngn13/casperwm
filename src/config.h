#pragma once

#include <X11/X.h>
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
  KeyCode code;
  
  char* mod;
  bool has_mod;
  int modc;

  char* action;
  char* argument;
};

struct Config {
  bool debug;
  char* mod;
  char* bar;
  int float_step;

  char* border_active;
  char* border_inactive;
  int border_width;

  int margin;
  int workspaces;
  int binding_count;
  struct Binding* bindings;
};

void init_cfg();
void free_cfg();
bool parse_cfg();

extern struct Config cfg;
