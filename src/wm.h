#pragma once

#include <X11/Xlib.h>
#include <stdbool.h>

struct WM {
  Display* dp;
  Window root;
  int sc;

  int dp_width;
  int dp_height;

  int workspace;
  bool found_wm;
  bool quit;
};

extern struct WM wm;
