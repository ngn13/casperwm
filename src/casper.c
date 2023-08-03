/* 
 *
 * casperwm | a simple x11 window manager
 * copyright 2023 ngn (ngn13proton@proton.me) 
 * https://ngn13.fun/l/capserwm
 * 
 * This project is under GPLv2 
 * (GNU Public License Version 2)
 * Please see LICENSE.txt
 *
 */

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

// defining the WM object
// struct is defined in wm.h
struct WM wm = {
  .max_workspace = WORKSPACE_COUNT,
  .wm_found = false, 
  .current_workspace = 1,
  .active = -1,
  .quit = false
};

// handles errors
int handler(Display* d, XErrorEvent* e){
  // if error is BadAccess, this means
  // theres already a running wm 
  // if thats the case we can exit
  if(e->error_code == BadAccess){
    wm.wm_found = true;
    return 0;
  }

  char m[100];
  sprintf(m, "Handler error! Code: %d", e->error_code);
  error(m);
  return 0;
}

// loop function, listens for events 
// and calls releated functions
int loop(){
  XEvent e;
  XNextEvent(wm.d, &e);

  switch(e.type){
    // gets called when a window is created
    case CreateNotify:
      event_create(&wm, e.xcreatewindow);
      break;
    // gets called when a window requests config (resize etc.)
    case ConfigureRequest:
      event_config(&wm, e.xconfigurerequest);
      break;
    // gets called when a window requests mapping (mapping makes the window visible)
    case MapRequest:
      event_map(&wm, e.xmaprequest);
      break;
    // gets called when a window reqeusts unmapping (makes the window invisible)
    case UnmapNotify:
      event_unmap(&wm, e.xunmap);
      break;
    // gets called when a window is destroyed
    case DestroyNotify:
      event_destroy(&wm, e.xdestroywindow);
      break;
    // gets called when a grabbed key is pressed
    case KeyPress:
      event_key(&wm, e.xkey);
      break;
    // gets called when a grabbed mouse button is pressed
    case ButtonPress:
      event_button(&wm, e.xbutton);
      break;
  }

  if(wm.quit){
    return -1;
  }

  return 0;
}

// closes display
void finish(){
  XCloseDisplay(wm.d);
  success("Closed the display");
}

// main function
int main(){
  // empty list of clients (yes max is 100 and its hardcoded)
  wm.clients = malloc(100*sizeof(struct Client));
  wm.d = XOpenDisplay(NULL);

  // if cant open display just quit
  if(wm.d==NULL){
    error("Error opening X display");
    return EXIT_FAILURE;
  }

  // setting the handlers + defining the root window
  XSetErrorHandler(&handler);
  wm.r = DefaultRootWindow(wm.d);
  XSelectInput(
      wm.d,
      wm.r,
      SubstructureRedirectMask | SubstructureNotifyMask);
  XSync(wm.d, false);

  // if wm found (see the error handler) then exit
  if (wm.wm_found){
    error("There is already a running window manager");
    finish();
    return EXIT_FAILURE;
  }

  success("Created display");

  // grab all the keys we need, MOD specified in config
  XGrabKey(wm.d, XKeysymToKeycode(wm.d, XStringToKeysym("Return")), MOD, wm.r, True, GrabModeAsync, GrabModeAsync);
  XGrabKey(wm.d, XKeysymToKeycode(wm.d, XStringToKeysym("p")), MOD, wm.r, True, GrabModeAsync, GrabModeAsync);
  XGrabKey(wm.d, XKeysymToKeycode(wm.d, XStringToKeysym("c")), MOD, wm.r, True, GrabModeAsync, GrabModeAsync);
  XGrabKey(wm.d, XKeysymToKeycode(wm.d, XStringToKeysym("q")), MOD, wm.r, True, GrabModeAsync, GrabModeAsync);

  for(int i = 1; i<=wm.max_workspace; i++){
    char key[10];
    sprintf(key, "%d", i);
    XGrabKey(wm.d, XKeysymToKeycode(wm.d, XStringToKeysym(key)), MOD, wm.r, True, GrabModeAsync, GrabModeAsync);
  }

  // grab the left click
  XGrabButton(wm.d, 1, AnyModifier, wm.r, True,
            ButtonPressMask, GrabModeSync, GrabModeAsync, None, None);

  info("Entering event loop");

  // running the startup function in a fork 
  // also defined in config
  pid_t pid = fork();
  if(pid == 0){
    daemon(0,0);
    startup();
    exit(0);
  }

  // entering the event loop here, will stay in 
  // the loop till the event loop returns -1
  while (true) {
    int code = loop();
    if (code==-1)
      break;
  }

  // when we are done do cleanup and exit
  finish();
  info("Cleaning up");
  free(wm.clients);
  return EXIT_SUCCESS;
}
