#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <stdbool.h>

#include "resolver.h"
#include "wm.h"
#include "log.h"

struct WMWindow* windows = NULL;
struct WM wm = {
  .found_wm = false,
  .workspace = 1,
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
        windows[i].workspace == wm.workspace &&
        !windows[i].isfloat ) {
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
    height = wm.dp_height-wm.bar_height-(cfg.margin*2)-(cfg.border_width*2);

    posy = cfg.margin+wm.bar_height+cfg.border_width;
    posx = prevx+(cfg.margin/2)+(cfg.border_width/2);
    width = width-cfg.margin-(cfg.border_width*2);

    if (i == count-1){
      width -= cfg.margin+cfg.border_width;
    }

    if (i == 0){
      posx += (cfg.margin/2)+(cfg.border_width/2);
    }

    XMoveWindow(wm.dp, active_windows[i].w, posx, posy);
    XResizeWindow(wm.dp, active_windows[i].w, width, height);
    prevx = posx+width+(cfg.margin/2)+(cfg.border_width/2); 
  }

  free(active_windows);
  return true;
}

void focus(Window w) {
  if(wm.active != 0 && strlen(cfg.border_inactive)!= 0 && cfg.border_width != 0) {
    debug("Setting inactive border");
    XSetWindowBorder(wm.dp, wm.active, color_resolver(cfg.border_inactive));
    XConfigureWindow(wm.dp, wm.active, CWBorderWidth, &(XWindowChanges){.border_width = cfg.border_width});
  }

  if(strlen(cfg.border_active)!=0 && cfg.border_width != 0) {
    debug("Setting active border");
    XSetWindowBorder(wm.dp, w, color_resolver(cfg.border_active));
    XConfigureWindow(wm.dp, w, CWBorderWidth, &(XWindowChanges){.border_width = cfg.border_width});
  }
  
  wm.active = w;
  XSetInputFocus(wm.dp, w, RevertToParent, CurrentTime);
  XRaiseWindow(wm.dp, w);
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
  XWindowAttributes* attr = malloc(sizeof(XWindowAttributes));
  XFetchName(wm.dp, w, &name);
  XGetWindowAttributes(wm.dp, w, attr);

  if(strstr(name, cfg.bar)!=NULL) {
    wm.bar_height = attr->y+attr->height;
    wm.bar = w;
    XMapWindow(wm.dp, w); 
    free(attr);
    XFree(name);
    return true;
  }
  
  free(attr);
  XFree(name);
  
  XSizeHints* sh = malloc(sizeof(XSizeHints));
  XGetNormalHints(wm.dp, w, sh);

  bool flt = false;
  if((sh->height <= 200 && sh->height != 0) || (sh->width <= 200 && sh->width != 0)) {
    flt = true;
  }

  struct WMWindow window = {
    .w = w,
    .state = MAPPED,
    .workspace = wm.workspace,
    .isfloat = flt,
  };
  
  free(sh);

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
    if( windows[i].state == MAPPED && 
        windows[i].w != w && 
        windows[i].workspace == wm.workspace) {
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

void remap_all() {
  for(int i = 0; i < wm.windowc-1; i++) {
    if( windows[i].state == MAPPED &&
        windows[i].workspace != wm.workspace) {
      windows[i].state = UNMAPPED;
      XUnmapWindow(wm.dp, windows[i].w);
    }else if ( windows[i].state == UNMAPPED &&
               windows[i].workspace == wm.workspace) {
      windows[i].state = MAPPED;
      XMapWindow(wm.dp, windows[i].w);
      focus(windows[i].w);
    }
  }
}

bool change_workspace(int workspace) {
  if(workspace > cfg.workspaces) {
    return false;
  }

  debug("Changing to workspace: %d", workspace);

  wm.workspace = workspace;
  remap_all();

  return true;
}

bool move_workspace(int workspace) {
  if(workspace > cfg.workspaces) {
    return false;
  }

  for(int i = 0; i < wm.windowc-1; i++) {
    if( windows[i].w == wm.active &&
        windows[i].workspace != workspace) {
      debug("Moving %d to workspace: %d", wm.active, workspace);
      windows[i].workspace = workspace;
      break;
    }
  }
  remap_all();

  return true;
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

void toggle_float(){
  if(wm.active == 0) {
    return;
  }
  
  for(int i = 0; i < wm.windowc-1; i++){
    if(windows[i].w == wm.active) {
      debug("Toggling float for %d", wm.active);
      windows[i].isfloat = !windows[i].isfloat;
      resize_windows();
    }
  }
}

void resize_window(char* dr) {
  if(wm.active == 0) {
    return;
  }

  for(int i = 0; i < wm.windowc-1; i++) {
    if( windows[i].w == wm.active &&
        !windows[i].isfloat ) {
      return;
    }
  }
  
  XWindowAttributes* attr = malloc(sizeof(XWindowAttributes));
  XGetWindowAttributes(wm.dp, wm.active, attr);
  
  #define CHECK(d, s) strcmp(d, s)==0
  if(CHECK(dr, "shrink_down")){
    XResizeWindow(wm.dp, wm.active, 
        attr->width, attr->height-cfg.float_step);
  }else if(CHECK(dr, "shrink_left")){
    XResizeWindow(wm.dp, wm.active, 
        attr->width-cfg.float_step, attr->height);
  }else if(CHECK(dr, "shrink")){
    XResizeWindow(wm.dp, wm.active, 
        attr->width-cfg.float_step, attr->height-cfg.float_step);
  }else if(CHECK(dr, "grow_down")){
    XResizeWindow(wm.dp, wm.active, 
        attr->width, attr->height+cfg.float_step);
  }else if(CHECK(dr, "grow_left")){
    XResizeWindow(wm.dp, wm.active, 
        attr->width+cfg.float_step, attr->height);
  }else if(CHECK(dr, "grow")){
    XResizeWindow(wm.dp, wm.active, 
        attr->width+cfg.float_step, attr->height+cfg.float_step);
  }else {
    print("WM", "Unknown option for resize: %s", dr);
  }

  free(attr);
  return; 
}

void move_window(char* dr) {
  if(wm.active == 0) {
    return;
  }

  for(int i = 0; i < wm.windowc-1; i++) {
    if( windows[i].w == wm.active &&
        !windows[i].isfloat ) {
      return;
    }
  }
  
  XWindowAttributes* attr = malloc(sizeof(XWindowAttributes));
  XGetWindowAttributes(wm.dp, wm.active, attr);
  
  #define CHECK(d, s) strcmp(d, s)==0
  if(CHECK(dr, "left")){
    XMoveWindow(wm.dp, wm.active, 
        attr->x-cfg.float_step, attr->y);
  }else if(CHECK(dr, "right")){
    XMoveWindow(wm.dp, wm.active, 
        attr->x+cfg.float_step, attr->y);
  }else if(CHECK(dr, "up")){
    XMoveWindow(wm.dp, wm.active, 
        attr->x, attr->y-cfg.float_step);
  }else if(CHECK(dr, "down")){
    XMoveWindow(wm.dp, wm.active, 
        attr->x, attr->y+cfg.float_step);
  }else {
    print("WM", "Unknown option for move: %s", dr);
  }

  free(attr);
  return;
}
