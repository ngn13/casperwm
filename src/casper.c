#include <X11/Xlib.h>
#include <signal.h>

#include "log.h"
#include "resolver.h"
#include "wm.h"
#include "config.h"
#include "handler.h"
#include "startup.h"

struct EventMapping map[] = {
  {.event = KeyPress, .func = key_handler},
  {.event = MapRequest, .func = map_handler},
  {.event = DestroyNotify, .func = destroy_handler},
  {.event = UnmapNotify, .func = unmap_handler},
};

void clean() {
  print("MAIN", "Cleaning up");
  clean_windows();
  XCloseDisplay(wm.dp);
  free_cfg();
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

  int test = XKeysymToKeycode(wm.dp, XStringToKeysym("e"));
  XGrabKey(wm.dp, test, Mod4Mask, wm.root, 
      True, GrabModeAsync, GrabModeAsync);

  wm.mod = mod_resolver(cfg.mod);
  for(int i = 0; i < cfg.binding_count; i++) {
    cfg.bindings[i].code = XKeysymToKeycode(wm.dp, XStringToKeysym(cfg.bindings[i].key));
    
    if(cfg.bindings[i].has_mod) {
      cfg.bindings[i].modc = mod_resolver(cfg.bindings[i].mod);
      XGrabKey(wm.dp, cfg.bindings[i].code, wm.mod|cfg.bindings[i].modc, wm.root, 
        True, GrabModeAsync, GrabModeAsync);
    }

    XGrabKey(wm.dp, cfg.bindings[i].code, wm.mod, wm.root, 
        True, GrabModeAsync, GrabModeAsync);
  }  

  signal(SIGINT, SIG_IGN);
  run_startup();

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
