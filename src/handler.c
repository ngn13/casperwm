#include <X11/Xlib.h>
#include <stdbool.h>
#include <wordexp.h>

#include "log.h"
#include "resolver.h"
#include "wm.h"

int error_handler(Display* dp, XErrorEvent* err) {
  if(wm.quit) {
    return 0;
  }

  if(err->error_code == BadAccess) {
    wm.found_wm = true;
    return 0;
  }
  
  print("HANDLER", "Handler got an error: %d", err->error_code);
  return 0;
}

bool bind_handler(struct Binding b) {
  #define ACTION(b, a) strcmp(b.action, a)==0

  if(ACTION(b, "cmd")) {
    wordexp_t words;
    wordexp(b.argument, &words, 0);
    
    if(fork() == 0){
      if(wm.dp) close(ConnectionNumber(wm.dp));
      daemon(1, 0);
      setsid();
      execvp(words.we_wordv[0], words.we_wordv);
      exit(0);
    }

    wordfree(&words);
  }else if(ACTION(b, "close")) {
    close_active();
  }else if(ACTION(b, "quit")) {
    wm.quit = true;
    close_active();
  }else if(ACTION(b, "focus")) {
    focus_next_window();
  }

  return true;
}

bool key_handler(XEvent event){
  XKeyEvent e = event.xkey;

  for(int i = 0; i < cfg.binding_count; i++) {
    if(cfg.bindings[i].code == e.keycode) {
      bind_handler(cfg.bindings[i]);
    }
  }

  return true;
}

bool map_handler(XEvent event) {
  XMapRequestEvent e = event.xmaprequest;

  if(!alloc_windows()) {
    print("HANDLER", "Cannot allocate windows");
    return false;
  }

  //XSelectInput(wm.dp, e.window, StructureNotifyMask|EnterWindowMask);
  add_window(e.window);
  return true;
}

bool destroy_handler(XEvent event) {
  XDestroyWindowEvent e = event.xdestroywindow;

  destroy_window(e.window);
  return true;
}

bool unmap_handler(XEvent event) {
  XUnmapEvent e = event.xunmap;

  unmap_window(e.window);
  return true;
}
