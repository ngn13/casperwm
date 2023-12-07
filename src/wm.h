#pragma once

#include <X11/Xlib.h>
#include <stdbool.h>

#define DEAD 0
#define MAPPED 1
#define UNMAPPED 2 
#define UNMAPPED_SELF 3

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
  bool isfloat;
  int state;
};

extern struct WMWindow* windows;
extern struct WM wm;

bool alloc_windows();
bool clean_windows();
bool add_window(Window);
bool destroy_window(Window);
bool change_workspace(int);
bool move_workspace(int);
bool unmap_window(Window);
bool focus_next_window();
void toggle_float();
void resize_window(char*);
void move_window(char*);
void focus(Window w);
void close_active();
