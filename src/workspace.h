#ifndef WORKSPACE_H
#define WORKSPACE_H
// ################################
#include "wm.h"
#include <X11/X.h>

void add_to_workspace(struct Workspace, struct Client);
struct Client* get_from_workspace(struct Workspace, Window);
void remove_from_workspace(struct Workspace, struct Client);
void unrender(struct WM*);
void render(struct WM*);

// ################################
#endif
