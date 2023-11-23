#include <X11/Xlib.h>
#include <stdbool.h>

#include "wm.h"
#include "log.h"

struct WM wm = {
  .found_wm = false,
  .workspace = 0,
  .quit = false,
};


