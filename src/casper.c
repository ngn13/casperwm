#include <X11/X.h>
#include <X11/Xlib.h> 
#include <X11/Xutil.h> 
#include <assert.h>   
#include <unistd.h>   
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>

#include "log.h"
#include "event.h"
#include "config.h"
#include "wm.h"

struct WM wm = {
  .wm_found = false, 
  .current_workspace = 0,
  .active = -1, 
};

int handler(Display* d, XErrorEvent* e){
  if(e->error_code == BadAccess){
    wm.wm_found = true;
    return 0;
  }

  char m[100];
  sprintf(m, "Handler error! Code: %d", e->error_code);
  error(m);
  return 0;
}

int loop(){
  XEvent e;
  XNextEvent(wm.d, &e);

  switch(e.type){
    case CreateNotify:
      event_create(&wm, e.xcreatewindow);
      break;
    case ConfigureRequest:
      event_config(&wm, e.xconfigurerequest);
      break;
    case MapRequest:
      event_map(&wm, e.xmaprequest);
      break;
    case UnmapNotify:
      event_unmap(&wm, e.xunmap);
      break;
    case DestroyNotify:
      event_destroy(&wm, e.xdestroywindow);
      break;
    case KeyPress:
      event_key(&wm, e.xkey);
      break;
    case ButtonPress:
      event_button(&wm, e.xbutton);
      break;
  }

  /*for( int i = 0; i<100; i++){
    printf("%lu ", wm.clients[i].w);
  }
  printf("\n");*/

  return 0;
}

void finish(){
  XCloseDisplay(wm.d);
  for(int i = 1; i<=WORKSPACE_COUNT; i++){
    free(wm.workspaces[i].clients);
  }
  free(wm.workspaces);
  success("Closed the display");
}

int main(){
  wm.d = XOpenDisplay(NULL);

  if(wm.d==NULL){
    error("Error opening X display");
    return EXIT_FAILURE;
  }

  XSetErrorHandler(&handler);
  wm.r = DefaultRootWindow(wm.d);
  XSelectInput(
      wm.d,
      wm.r,
      SubstructureRedirectMask | SubstructureNotifyMask);
  XSync(wm.d, false);

  if (wm.wm_found){
    error("There is already a running window manager");
    XCloseDisplay(wm.d);
    return EXIT_FAILURE;
  }

  success("Created display");
  XGrabKey(wm.d, XKeysymToKeycode(wm.d, XStringToKeysym("c")), MOD, wm.r, True, GrabModeAsync, GrabModeAsync);
  XGrabKey(wm.d, XKeysymToKeycode(wm.d, XStringToKeysym("q")), MOD, wm.r, True, GrabModeAsync, GrabModeAsync);
  XGrabKey(wm.d, XKeysymToKeycode(wm.d, XStringToKeysym("Return")), MOD, wm.r, True, GrabModeAsync, GrabModeAsync);
  XGrabKey(wm.d, XKeysymToKeycode(wm.d, XStringToKeysym("n")), MOD, wm.r, True, GrabModeAsync, GrabModeAsync);
  XGrabKey(wm.d, XKeysymToKeycode(wm.d, XStringToKeysym("p")), MOD, wm.r, True, GrabModeAsync, GrabModeAsync);
  XGrabButton(wm.d, 1, AnyModifier, wm.r, True,
            ButtonPressMask, GrabModeSync, GrabModeAsync, None, None);

  wm.workspaces = malloc(sizeof(struct Workspace)*WORKSPACE_COUNT);
  for(int i = 0; i<=WORKSPACE_COUNT; i++){
    char key[10];
    sprintf(key, "%d", i+1);
    XGrabKey(wm.d, XKeysymToKeycode(wm.d, XStringToKeysym(key)), MOD, wm.r, True, GrabModeAsync, GrabModeAsync);

    struct Workspace workspace;
    workspace.clients = malloc(sizeof(struct Client)*100);
    workspace.layout = 0;
    wm.workspaces[i] = workspace;
  }

  info("Running startup fork");
  pid_t pid = fork();
  if(pid == 0){
    daemon(0,0);
    startup();
    exit(0);
  }

  info("Entering event loop");
  while (true) {
    int code = loop();
    if (code==-1)
      break;
  }

  info("Cleaning up");
  finish();
  return EXIT_SUCCESS;
}
