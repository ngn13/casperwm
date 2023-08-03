#ifndef WM_H
#define WM_H
// #################################

#include <X11/Xlib.h> 
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>

#define DEAD 69
#define ALIVE 420
#define ZOMBIE 31

struct Client{
  Window w;
  int state;
  int workspace;
  bool mapped;
};

struct WM {
  Display* d;
  Window r;
  struct Client* clients;
  bool wm_found;
  int current_workspace;
  int max_workspace;
  Window bar;
  Window active;
  bool quit;
};

// #################################
#endif
