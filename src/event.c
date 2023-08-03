#include <X11/X.h>
#include <X11/Xlib.h>
#include <X11/Xatom.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include "event.h"
#include "render.h"
#include "config.h"
#include "wm.h"
#include "log.h"

void event_create(struct WM* wm, XCreateWindowEvent e){
  struct Client c = {
    .w = e.window, 
    .workspace = wm->current_workspace, 
    .state = ZOMBIE,
    .mapped = false
  };

  for(int i = 0; i<100; i++){
    if(wm->clients[i].state == DEAD || wm->clients[i].state == 0){
      wm->clients[i] = c;
      break;
    }
  }

  char m[100];
  sprintf(m, "Created window | ID: %lu", e.window);
  debug(m);
}

void event_map(struct WM* wm, XMapRequestEvent e){
  map(wm, e.window);

  resize(wm); 
  char m[100];
  sprintf(m, "Mapped window | ID: %lu", e.window);
  debug(m);
};

void event_config(struct WM* wm, XConfigureRequestEvent e){
  XWindowChanges c;
  c.x = e.x; c.y = e.y; c.height = e.height; c.width = e.width; c.sibling = e.above; c.stack_mode = e.detail; 
  c.border_width = e.border_width;
  XConfigureWindow(wm->d, e.window, e.value_mask, &c);

  resize(wm);

  char m[100];
  sprintf(m, "Configured window | ID: %lu", e.window);
  debug(m);
};

void event_unmap(struct WM* wm, XUnmapEvent e){
  unmap(wm, e.window);

  resize(wm);
  focus(wm);

  char m[100];
  sprintf(m, "Unmapped window | ID: %lu", e.window);
  debug(m);
}

void event_destroy(struct WM* wm, XDestroyWindowEvent e){
  for(int i = 0; i<100; i++){
    if(wm->clients[i].w == e.window){
      wm->clients[i].state = DEAD;
      wm->clients[i].mapped = false;
      wm->clients[i].w = 1;
      break;
    } 
  }

  resize(wm);
  focus(wm);

  char m[100];
  sprintf(m, "Destroyed window | ID: %lu", e.window);
  debug(m);
}

void event_button(struct WM* wm, XButtonEvent e){
  XRaiseWindow(wm->d, e.subwindow);
  XSetInputFocus(wm->d, e.subwindow, RevertToParent, CurrentTime);

  XAllowEvents(wm->d, ReplayPointer, CurrentTime);
  XSync(wm->d, 0);

  wm->active = e.subwindow;
}

void event_key(struct WM* wm, XKeyEvent e){
  if(e.keycode == XKeysymToKeycode(wm->d, XStringToKeysym("Return"))){
    pid_t pid = fork();
    if(pid == 0){
      daemon(0,0);
      system(TERMINAL);
      exit(0);
    }
    return;
  }
  
  else if(e.keycode == XKeysymToKeycode(wm->d, XStringToKeysym("p"))){
    pid_t pid = fork();
    if(pid == 0){
      daemon(0,0);
      system(LAUNCHER);
      exit(0);
    }
    return;
  }

  else if(e.keycode == XKeysymToKeycode(wm->d, XStringToKeysym("c"))){
    Window win;
    int revert_to;
    XGetInputFocus(wm->d, &win, &revert_to);
    if(win != None){
      XEvent out;
      out.xclient.type = ClientMessage;
      out.xclient.window = win;
      out.xclient.message_type = XInternAtom(wm->d, "WM_PROTOCOLS", true);
      out.xclient.format = 32;
      out.xclient.data.l[0] = XInternAtom(wm->d, "WM_DELETE_WINDOW", false);
      out.xclient.data.l[1] = CurrentTime; 
      XSendEvent(wm->d, win, false, NoEventMask, &out);
      //XDestroyWindow(wm->d, win);
    }
    return;
  }

  else if(e.keycode == XKeysymToKeycode(wm->d, XStringToKeysym("q"))){
    success("Exit with code 0");
    wm->quit = true;
    return;
  }

  for(int i = 1; i<=wm->max_workspace; i++){
    char key[10];
    sprintf(key, "%d", i);

    if(e.keycode != XKeysymToKeycode(wm->d, XStringToKeysym(key))){
      continue;
    }

    if(wm->current_workspace == i){
      break;
    }

    wm->current_workspace = i;

    for(int i = 0; i<100; i++){
      if(wm->clients[i].state == ALIVE && wm->clients[i].workspace == wm->current_workspace){
        map(wm, wm->clients[i].w);
        focus(wm);
      }else if(wm->clients[i].state == ALIVE && wm->clients[i].workspace != wm->current_workspace) {
        unmap(wm, wm->clients[i].w);
      }
    }

    resize(wm);

  }
}
