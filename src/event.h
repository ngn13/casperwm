#ifndef EVENT_H
#define EVENT_H
// #################################

#include <X11/Xlib.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include "wm.h"

void event_create(struct WM*, XCreateWindowEvent);
void event_config(struct WM*, XConfigureRequestEvent);
void map(struct WM*, Window);
void event_map(struct WM*, XMapRequestEvent);
void event_unmap(struct WM*, XUnmapEvent);
void event_destroy(struct WM*, XDestroyWindowEvent);
void event_key(struct WM*, XKeyEvent);
void event_button(struct WM*, XButtonEvent);

// #################################
#endif 
