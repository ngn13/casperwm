#include <X11/X.h>
#include <X11/Xlib.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>

#include "event.h"
#include "config.h"
#include "wm.h"
#include "log.h"
#include "workspace.h"
#include "layouts.h"

void event_create(struct WM* wm, XCreateWindowEvent e){
  struct Client client = {
    .w = e.window, 
    .state = ZOMBIE,  
    .mapped = false, 
    .workspace = wm->current_workspace
  };
  add_to_workspace(wm->workspaces[wm->current_workspace], client);

  char m[100];
  sprintf(m, "Created window | ID: %lu", e.window);
  success(m);
}


void map(struct WM* wm, Window w){
  char *name = NULL;
  XFetchName(wm->d, w, &name);

  if (name != NULL) {
    if(strstr(name, BAR)!=NULL){
      XMapWindow(wm->d, w);
      XFree(name);   
      return;
    }
    XFree(name);   
  }

  struct Client* client = NULL;

  for(int i = 0; i < WORKSPACE_COUNT; i++){
    client = get_from_workspace(wm->workspaces[i], w);
    if(client != NULL){
      break;
    }
  }
  
  if(client == NULL){
    return;
  }

  if(client->state == ZOMBIE){
    client->state = ALIVE;
  }

  XMapWindow(wm->d, w);
  client->mapped = true;

  char m[100];
  sprintf(m, "Mapped window | ID: %lu", w);
  success(m);
}

void event_map(struct WM* wm, XMapRequestEvent e){
  map(wm, e.window);
  call_layout(wm);
};

void event_config(struct WM* wm, XConfigureRequestEvent e){
  XWindowChanges c;
  c.x = e.x; c.y = e.y; c.height = e.height; c.width = e.width; c.sibling = e.above; c.stack_mode = e.detail; 
  c.border_width = e.border_width;
  XConfigureWindow(wm->d, e.window, e.value_mask, &c);
  call_layout(wm);

  char m[100];
  sprintf(m, "Configured window | ID: %lu", e.window);
  success(m);
};

void event_unmap(struct WM* wm, XUnmapEvent e){
  struct Client* client = NULL;

  for(int i = 0; i < WORKSPACE_COUNT; i++){
    client = get_from_workspace(wm->workspaces[i], e.window);
    if(client != NULL){
      break;
    }
  }
  
  if(client == NULL){
    return;
  }

  client->mapped = false;
  call_layout(wm);

  char m[100];
  sprintf(m, "Unmapped window | ID: %lu", e.window);
  success(m);
}

void event_destroy(struct WM* wm, XDestroyWindowEvent e){
  struct Client* client = get_from_workspace(wm->workspaces[wm->current_workspace], e.window);
  remove_from_workspace(wm->workspaces[wm->current_workspace], *client);
  call_layout(wm);

  char m[100];
  sprintf(m, "Destroyed window | ID: %lu", e.window);
  success(m);
}

void event_button(struct WM* wm, XButtonEvent e){
  if(e.subwindow != wm->active){
    XRaiseWindow(wm->d, e.subwindow);
    XSetInputFocus(wm->d, e.subwindow, RevertToParent, CurrentTime);
    wm->active = e.subwindow;
  }
  XAllowEvents(wm->d, ReplayPointer, CurrentTime);
  XSync(wm->d, 0);
}

void event_key(struct WM* wm, XKeyEvent e){
  if(e.keycode == XKeysymToKeycode(wm->d, XStringToKeysym("Return"))){
    pid_t pid = fork();
    if(pid == 0){
      daemon(0,0);
      system(TERMINAL);
      exit(0);
    }
  }
  
  else if(e.keycode == XKeysymToKeycode(wm->d, XStringToKeysym("p"))){
    pid_t pid = fork();
    if(pid == 0){
      daemon(0,0);
      system(LAUNCHER);
      exit(0);
    }
  }
 
  else if(e.keycode == XKeysymToKeycode(wm->d, XStringToKeysym("c"))){
    Window win;
    int revert_to;
    XGetInputFocus(wm->d, &win, &revert_to);
    if(win != None){
      XDestroyWindow(wm->d, win);
    }
  }

  else if(e.keycode == XKeysymToKeycode(wm->d, XStringToKeysym("q"))){
    success("Exit with code 0");
    exit(0);
  }

  else if(e.keycode == XKeysymToKeycode(wm->d, XStringToKeysym("n"))){
    switch_layout(wm);
    render(wm);
  }

  for(int i = 0; i<=WORKSPACE_COUNT; i++){
    char key[10];
    sprintf(key, "%d", i+1);
    if(e.keycode != XKeysymToKeycode(wm->d, XStringToKeysym(key))){
      continue;
    }
   
    if(i == wm->current_workspace){
      return;
    }

    unrender(wm);
    wm->current_workspace = i;
    render(wm);
  }
}
