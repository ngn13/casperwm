#include "layouts.h"
#include "config.h"
#include "wm.h"

struct ClientsList {
  int count;
  struct Client* list;
};

struct Layout {
  int layout;
  void* func;
};

struct ClientsList get_clients(struct Workspace workspace){
  struct ClientsList clients;

  clients.count = 0;
  clients.list = malloc(sizeof(struct Client)*100);

  for(int i = 0; i<100; i++){
    if( workspace.clients[i].state == ALIVE && workspace.clients[i].mapped ){
      clients.list[clients.count] = workspace.clients[i];
      clients.count += 1;
    }
  }
  
  return clients;
}

void switch_layout(struct WM* wm){
  wm->workspaces[wm->current_workspace].layout += 1;
  if(wm->workspaces[wm->current_workspace].layout >= 3){
    wm->workspaces[wm->current_workspace].layout = 0;
  }
  call_layout(wm);
}

void call_layout(struct WM* wm){
  switch (wm->workspaces[wm->current_workspace].layout) {
    case 0:
      vertical_layout(wm);
      break;
    case 1:
      horizantal_layout(wm);
      break;
    case 2:
      focused_layout(wm);
      break;
    default:
      break;
  }
}

void vertical_layout(struct WM* wm){
  struct ClientsList clients = get_clients(wm->workspaces[wm->current_workspace]);

  if(clients.count == 0){
    free(clients.list);
    return;
  }

  int prevx = 0;
  int num = DefaultScreen(wm->d);
  unsigned int width, height, posx, posy;

  for(int i = 0; i<clients.count; i++){
    width = (DisplayWidth(wm->d, num)/clients.count);
    height = DisplayHeight(wm->d, num)-(MARGIN_TOP)-(MARGIN);

    posy = MARGIN_TOP;
    posx = prevx+MARGIN/2;
    width = width-MARGIN;

    if (i == clients.count-1){
      width -= MARGIN;
    }

    if (i == 0){
      posx += MARGIN/2;
    }

    XMoveWindow(wm->d, clients.list[i].w, posx, posy);
    XResizeWindow(wm->d, clients.list[i].w, width, height);
    prevx = posx+width+MARGIN/2;
    XSetInputFocus(wm->d, clients.list[i].w, RevertToParent, CurrentTime);
  }
  
  free(clients.list);
}

void horizantal_layout(struct WM* wm){
  struct ClientsList clients = get_clients(wm->workspaces[wm->current_workspace]);

  if(clients.count == 0){
    free(clients.list);
    return;
  }

  int prevy = 0;
  int screen = DefaultScreen(wm->d);
  unsigned int width, height, posx, posy;

  for(int i = 0; i<clients.count; i++){
    width = DisplayWidth(wm->d, screen)-(MARGIN*2);
    height = ((DisplayHeight(wm->d, screen)-MARGIN_TOP)/clients.count);

    posy = prevy+MARGIN/2;
    posx = MARGIN;
    height = height-MARGIN;

    if (i == clients.count-1){
      height -= MARGIN;
    }

    if (i == 0){
      posy += MARGIN_TOP;
    }

    XMoveWindow(wm->d, clients.list[i].w, posx, posy);
    XResizeWindow(wm->d, clients.list[i].w, width, height);
    prevy = posy+height+MARGIN/2;
    XSetInputFocus(wm->d, clients.list[i].w, RevertToParent, CurrentTime);
  }
  
  free(clients.list);
}

void focused_layout(struct WM* wm){
  struct ClientsList clients = get_clients(wm->workspaces[wm->current_workspace]);

  if(clients.count == 0){
    free(clients.list);
    return;
  }

  Window focused = clients.list[0].w;
  for(int i = 0; i < clients.count; i++){
    if(clients.list[i].focused){
      focused = clients.list[i].w;
    }else if(focused != clients.list[0].w || i != 0){
      XUnmapWindow(wm->d, clients.list[i].w);
    }
  }

  int prevy = 0;
  int screen = DefaultScreen(wm->d);
  unsigned int width, height;
  width = DisplayWidth(wm->d, screen)-MARGIN;
  height = DisplayHeight(wm->d, screen)-MARGIN;

  XMoveWindow(wm->d, focused, MARGIN, MARGIN_TOP);
  XResizeWindow(wm->d, focused, width-MARGIN, height-MARGIN_TOP);
  XSetInputFocus(wm->d, focused, RevertToParent, CurrentTime);
  
  free(clients.list);
}
