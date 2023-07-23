#ifndef EVENT_H
#define EVENT_H
// #################################

#include <X11/Xlib.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include "wm.h"

void event_create(struct WM* wm, XCreateWindowEvent e);
void event_config(struct WM*, XConfigureRequestEvent);
void event_map(struct WM*, XMapRequestEvent);
void event_unmap(struct WM* wm, XUnmapEvent e);
void event_destroy(struct WM* wm, XDestroyWindowEvent e);
void event_key(struct WM* wm, XKeyEvent e);
void event_button(struct WM* wm, XButtonEvent e);

// #################################
#endif 
