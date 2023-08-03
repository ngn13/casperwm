#ifndef RENDER_H
#define RENDER_H
// #################################
#include "wm.h"
#include "config.h"
#include <string.h>

void focus(struct WM* wm);
void resize(struct WM* wm);
void map(struct WM* wm, Window w);
void unmap(struct WM* wm, Window w);

// #################################
#endif
