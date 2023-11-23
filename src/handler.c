#include <X11/Xlib.h>
#include <stdbool.h>

#include "handler.h"
#include "log.h"
#include "wm.h"

int error_handler(Display* dp, XErrorEvent* err) {
  if(err->error_code == BadAccess) {
    wm.found_wm = true;
    return 0;
  }
  
  print("HANDLER", "Handler got an error: %d", err->error_code);
  return 0;
}

bool key_handler(XEvent event){
  XKeyEvent e = event.xkey;

  print("HANDLER", "Got key: %d", e.keycode);
  return 0;
}

bool btn_handler(XEvent event){
  XButtonEvent e = event.xbutton;

  print("HANDLER", "Got button: %d", e.button);
  return 0;
}

