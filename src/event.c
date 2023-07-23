#include <X11/X.h>
#include <X11/Xlib.h>
#include <X11/Xatom.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include "event.h"
#include "config.h"
#include "wm.h"
#include "log.h"

void resize(struct WM* wm){
  
  int count = 0;
  struct Client* clients = malloc(sizeof(struct Client)*100);

  for(int i = 0; i<100; i++){
    if(wm->clients[i].state == ALIVE && wm->clients[i].workspace == wm->current_workspace){
      clients[count] = wm->clients[i];
      count += 1;
    }
  }

  int prevx = 0;
  int num = DefaultScreen(wm->d);
  unsigned int width, height, posx, posy;

  for(int i = 0; i<count; i++){
    width = (DisplayWidth(wm->d, num)/count);
    height = DisplayHeight(wm->d, num)-(MARGIN_TOP)-(MARGIN);

    posy = MARGIN_TOP;
    posx = prevx+MARGIN/2;
    width = width-MARGIN;

    if (i == count-1){
      width -= MARGIN;
    }

    if (i == 0){
      posx += MARGIN/2;
    }

    XMoveWindow(wm->d, clients[i].w, posx, posy);
    XResizeWindow(wm->d, clients[i].w, width, height);
    prevx = posx+width+MARGIN/2;
  }
  
  free(clients);

}

void event_create(struct WM* wm, XCreateWindowEvent e){
  char m[100];
  sprintf(m, "Created window | ID: %lu", e.window);
  success(m);
}

void event_map(struct WM* wm, XMapRequestEvent e){
  char *name = NULL;
  XFetchName(wm->d, e.window, &name);
  if (name != NULL) {
    if(strstr(name, BAR)!=NULL){
      XMapWindow(wm->d, e.window);
      XFree(name);   
      return;
    }
    XFree(name);   
  }

  for(int i = 0; i<100; i++){
    if(wm->clients[i].w == e.window){
      XMapWindow(wm->d, e.window);
      return;
    }
  }

  struct Client c = {
    .w = e.window, 
    .workspace = wm->current_workspace, 
    .state = ALIVE,
  };

  for(int i = 0; i<100; i++){
    if(wm->clients[i].state == DEAD || wm->clients[i].state == 0){
      wm->clients[i] = c;
      break;
    }
  }

  XMapWindow(wm->d, e.window);
  resize(wm); 

  char m[100];
  sprintf(m, "Mapped window | ID: %lu", e.window);
  success(m);
};


void event_config(struct WM* wm, XConfigureRequestEvent e){
  XWindowChanges c;
  c.x = e.x; c.y = e.y; c.height = e.height; c.width = e.width; c.sibling = e.above; c.stack_mode = e.detail; 
  c.border_width = e.border_width;
  XConfigureWindow(wm->d, e.window, e.value_mask, &c);

  resize(wm); 
  char m[100];
  sprintf(m, "Configured window | ID: %lu", e.window);
  success(m);
};

void event_unmap(struct WM* wm, XUnmapEvent e){
  char m[100];
  sprintf(m, "Unmapped window | ID: %lu", e.window);
  success(m);
}

void event_destroy(struct WM* wm, XDestroyWindowEvent e){
  for(int i = 0; i<100; i++){
    if(wm->clients[i].w == e.window){
      wm->clients[i].state = DEAD;
      wm->clients[i].w = -1;
      break;
    } 
  }

  resize(wm); 
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
    XDestroyWindow(wm->d, win);
  }

  else if(e.keycode == XKeysymToKeycode(wm->d, XStringToKeysym("q"))){
    for(int i = 0; i<100; i++){
      if(wm->clients[i].state != DEAD && wm->clients[i].w != 0){
        printf("Killing %lu\n", wm->clients[i].w);
        XDestroyWindow(wm->d, wm->clients[i].w);
      }
    }
    success("Exit with code 0");
    exit(0);
  }

  else{
    for(int i = 1; i<=wm->max_workspace; i++){
      char key[10];
      sprintf(key, "%d", i);
      if(e.keycode != XKeysymToKeycode(wm->d, XStringToKeysym(key))){
        continue;
      }
      int prev_workspace = wm->current_workspace;
      wm->current_workspace = i;

      for(int i = 0; i<100; i++){
        if(wm->clients[i].state == ALIVE && wm->clients[i].workspace == wm->current_workspace){
          if(wm->current_workspace != prev_workspace){
            XMapWindow(wm->d, wm->clients[i].w);
            XRaiseWindow(wm->d, wm->clients[i].w);
            XSetInputFocus(wm->d, wm->clients[i].w, RevertToParent, CurrentTime);
            wm->active = wm->clients[i].w;
          }
        }else if(wm->clients[i].state == ALIVE && wm->clients[i].workspace != wm->current_workspace) {
          XUnmapWindow(wm->d, wm->clients[i].w);
        }
      }
      resize(wm);
    }
  }
}
