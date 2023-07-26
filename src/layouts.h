#ifndef LAYOUTS_H
#define LAYOUTS_H
// ###################################
#include "wm.h"

typedef void (layout)(struct WM*);
layout call_layout;
layout switch_layout;
layout vertical_layout;
layout horizantal_layout;
layout fullscreen_layout;
layout focused_layout;
layout floating_layout;

// ###################################
#endif
