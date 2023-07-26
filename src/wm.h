#ifndef WM_H
#define WM_H
// #################################

#include <X11/Xlib.h> 
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include "config.h"

#define DEAD 69
#define ZOMBIE 31
#define ALIVE 420

struct Client{
  int workspace;
  bool focused;
  bool mapped;
  int state;
  Window w;
};

struct Workspace{
  struct Client* clients;
  bool active;
  int layout;
};

struct WM {
  struct Workspace* workspaces;
  int current_workspace;
  bool wm_found;
  Window active;
  Window bar;
  Display* d;
  Window r;
};

// #################################
#endif
