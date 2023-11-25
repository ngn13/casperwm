#pragma once

#include <X11/Xlib.h>
#include <stdbool.h>

typedef bool(*event_handler)(XEvent event);

struct EventMapping {
  XEvent event;
  event_handler func;
};

int error_handler(Display* dp, XErrorEvent* err);
bool key_handler(XEvent event);
bool map_handler(XEvent event);
bool destroy_handler(XEvent event);
bool unmap_handler(XEvent event);
