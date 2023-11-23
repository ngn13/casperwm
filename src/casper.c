#include <X11/Xlib.h>
#include <signal.h>

#include "log.h"
#include "wm.h"
#include "config.h"
#include "handler.h"

struct EventMapping map[] = {
  {.event = KeyPress, .func = key_handler},
  {.event = ButtonPress, .func = btn_handler}
};

void clean() {
  print("MAIN", "Cleaning up");
  XCloseDisplay(wm.dp);
  free_cfg();
}

void signal_handler(int sig){
  print("MAIN", "Got interrupt");
  wm.quit = true;
}

int main() {
  print("MAIN", "Starting capserwm");
  init_cfg();
  parse_cfg();

  debug("Mod key: %s", cfg.mod);
  debug("Loaded total of %d bindings", cfg.binding_count);

  wm.dp = XOpenDisplay(NULL);
  if(wm.dp == NULL) {
    print("MAIN", "Cannot open the display");
    return EXIT_FAILURE;
  }

  wm.root = XDefaultRootWindow(wm.dp);
  XSetErrorHandler(error_handler);
  XSelectInput(wm.dp, wm.root, SubstructureRedirectMask | SubstructureNotifyMask);
  XSync(wm.dp, false);

  if(wm.found_wm) {
    print("MAIN", "There is an already running window manager");
    clean();
    return EXIT_FAILURE;
  }
  
  XDefineCursor(wm.dp, wm.root, XCreateFontCursor(wm.dp, 34));

  wm.sc = DefaultScreen(wm.dp); 
  wm.dp_width = XDisplayWidth(wm.dp, wm.sc);
  wm.dp_height = XDisplayHeight(wm.dp, wm.sc);

  for(int i = 0; i < cfg.binding_count; i++) {
    XGrabKey(wm.dp, XKeysymToKeycode(wm.dp, XStringToKeysym(cfg.bindings[i].key)), AnyModifier, wm.root, 
      True, GrabModeAsync, GrabModeAsync);
  } 

  XGrabButton(wm.dp, 1, AnyModifier, wm.root, True,
    ButtonPressMask|ButtonReleaseMask|PointerMotionMask,
    GrabModeAsync, GrabModeAsync, 0, 0);

  signal(SIGINT, signal_handler);

  XEvent event;
  while(!wm.quit && !XNextEvent(wm.dp, &event)) {
    for(int i = 0; i < sizeof(map)/sizeof(struct EventMapping); i++) {
      if(event.type == map[i].event.type){
        map[i].func(event);
        break;
      }
    }
  }

  clean();
  return EXIT_SUCCESS;
}
