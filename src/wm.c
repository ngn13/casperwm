#include <X11/Xlib.h>
#include <stdbool.h>

#include "wm.h"
#include "log.h"

// windows??? what??? omg!!1!1!1
struct WMWindow* windows = NULL;
struct WM wm = {
  .found_wm = false,
  .workspace = 0,
  .windowc = 0,
  .quit = false,
  .active = 0,
  .bar_height = 0,
  .bar = 0,
};

bool resize_windows() {
  int count = 1;
  struct WMWindow* active_windows = malloc(sizeof(struct WMWindow)*count);

  for(int i = 0; i < wm.windowc-1; i++) {
    if( windows[i].state == MAPPED && 
        windows[i].workspace == wm.workspace ) {
      active_windows[count-1] = windows[i];
      count++;
      active_windows = realloc(active_windows, sizeof(struct WMWindow)*count);
    } 
  }

  count--;
  debug("Resizing %d windows", count);
  
  unsigned int width, height, posx, posy;
  int prevx = 0;

  for(int i = 0; i < count; i++) {
    width = wm.dp_width/count;
    height = wm.dp_height-wm.bar_height-(cfg.margin*2);

    posy = cfg.margin+wm.bar_height;
    posx = prevx+cfg.margin/2;
    width = width-cfg.margin;

    if (i == count-1){
      width -= cfg.margin;
    }

    if (i == 0){
      posx += cfg.margin/2;
    }

    XMoveWindow(wm.dp, active_windows[i].w, posx, posy);
    XResizeWindow(wm.dp, active_windows[i].w, width, height);
    prevx = posx+width+cfg.margin/2; 
  }

  free(active_windows);
  return true;
}

void focus(Window w) {
  wm.active = w;
  XSetInputFocus(wm.dp, w, RevertToParent, CurrentTime);
}

bool focus_next_window() {
  int indx = 0;

  for(int i = 0; i < wm.windowc-1; i++) {
    if(wm.active == windows[i].w) {
      indx = i;
    }
  }

  for(;indx < wm.windowc-1; indx++) {
    if( windows[indx].w != wm.active &&
        windows[indx].state != DEAD &&
        windows[indx].workspace == wm.workspace) {
      focus(windows[indx].w);
      return true;
    }
  }

  for(int i = 0; i < wm.windowc-1; i++) {
    if( windows[indx].w != wm.active &&
        windows[i].state != DEAD &&
        windows[i].workspace == wm.workspace) {
      focus(windows[i].w);
      return true;
    }
  }

  return false;
}

void close_active() {
  if(wm.active == 0){
    return;
  }

  for(int i = 0; i < wm.windowc-1; i++){
    if(windows[i].state != DEAD && windows[i].w == wm.active) {
      XKillClient(wm.dp, wm.active);
      return;
    }
  }
}

bool alloc_windows() {
  if(windows == NULL) {
    wm.windowc++;
    windows = malloc(sizeof(struct WMWindow)*wm.windowc);
  }
  return windows != NULL;
}

bool add_window(Window w) {
  char *name = NULL;
  XWindowAttributes wa;
  XFetchName(wm.dp, w, &name);
  XGetWindowAttributes(wm.dp, w, &wa);

  if(strstr(name, cfg.bar)!=NULL) {
    wm.bar_height = wa.y+wa.height;
    wm.bar = w;
    XMapWindow(wm.dp, w);
    XFree(name);
    return true;
  }
  
  XFree(name);

  struct WMWindow window = {
    .w = w,
    .state = MAPPED,
    .workspace = wm.workspace
  };

  if(wm.windowc > 1) {
    for(int i = 0; i < wm.windowc-1; i++) {
      if(windows[i].state != DEAD) {
        continue;
      }

      debug("Overwriting window %d with %d", windows[i].w, w);
      windows[i] = window;
      XMapWindow(wm.dp, w);
      focus(w);
      resize_windows();
      return true;
    }
  }

  debug("Added window: %d", w);

  windows[wm.windowc-1] = window;
  wm.windowc++;
  windows = realloc(windows, sizeof(struct WMWindow)*wm.windowc);

  XMapWindow(wm.dp, w);
  focus(w);
  resize_windows();
  return true;
}

bool destroy_window(Window w) {
  for(int i = 0; i < wm.windowc-1; i++) {
    if(windows[i].w == w) {
      windows[i].state = DEAD;
      debug("Destroyed window: %d", w);
      return true;
    }
  }

  return false;
}

bool unmap_window(Window w) {
  for(int i = 0; i < wm.windowc-1; i++) {
    if(windows[i].state == MAPPED && windows[i].w != w) {
      focus(windows[i].w);
    }

    else if(windows[i].w == w) {
      windows[i].state = UNMAPPED;
      debug("Unmapped window: %d", w);
      resize_windows();
    }
  }

  return false;
}

bool clean_windows() {
  if(windows == NULL) {
    return false;
  }

  for(int i = 0; i < wm.windowc-1; i++) {
    if(windows[i].state != DEAD) {
      XKillClient(wm.dp, windows[i].w);
    }
  }

  if(wm.bar != 0) {
    XKillClient(wm.dp, wm.bar);
  }

  free(windows);
  return true;
}
