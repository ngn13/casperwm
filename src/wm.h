#pragma once

#include <X11/Xlib.h>
#include <stdbool.h>

#define DEAD 0
#define UNMAPPED 1 
#define MAPPED 2

struct WM {
  Display* dp;
  Window root;
  Window active;
  int sc;
  int mod;

  int dp_width;
  int dp_height;

  Window bar;
  int bar_height;

  int workspace;
  bool found_wm;
  int windowc;
  bool quit;
};

struct WMWindow {
  Window w;
  int workspace;
  int state;
};

extern struct WMWindow* windows;
extern struct WM wm;

bool alloc_windows();
bool clean_windows();
bool add_window(Window);
bool destroy_window(Window);
bool unmap_window(Window);
bool focus_next_window();
void focus(Window w);
void close_active();
