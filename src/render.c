#include "render.h"

void focus(struct WM* wm){
  for(int i = 0; i<100; i++){
    if(wm->clients[i].state == ALIVE && wm->clients[i].workspace == wm->current_workspace){
      XRaiseWindow(wm->d, wm->clients[i].w);
      XSetInputFocus(wm->d, wm->clients[i].w, RevertToParent, CurrentTime);
      wm->active = wm->clients[i].w;
    }
  }
}

void resize(struct WM* wm){
  int count = 0;
  struct Client* clients = malloc(sizeof(struct Client)*100);

  for(int i = 0; i<100; i++){
    if( wm->clients[i].state == ALIVE && 
        wm->clients[i].workspace == wm->current_workspace && 
        wm->clients[i].mapped)
    {
      clients[count] = wm->clients[i];
      count += 1;
    }
  }

  int prevx = 0;
  int num = DefaultScreen(wm->d);
  unsigned int width, height, posx, posy;

  for(int i = 0; i<count; i++){
    width = (DisplayWidth(wm->d, num)/count);
    height = DisplayHeight(wm->d, num)-(MARGIN_TOP)-(MARGIN);

    posy = MARGIN_TOP;
    posx = prevx+MARGIN/2;
    width = width-MARGIN;

    if (i == count-1){
      width -= MARGIN;
    }

    if (i == 0){
      posx += MARGIN/2;
    }

    XMoveWindow(wm->d, clients[i].w, posx, posy);
    XResizeWindow(wm->d, clients[i].w, width, height);
    prevx = posx+width+MARGIN/2;
  }
  
  free(clients);
}

void map(struct WM* wm, Window w){
  char *name = NULL;
  XFetchName(wm->d, w, &name);
  
  if (name != NULL) {
    if(strstr(name, BAR)!=NULL){
      XMapWindow(wm->d, w);
      XFree(name);   
      return;
    }
    XFree(name);   
  }
  
  for(int i = 0; i<100; i++){
    if(wm->clients[i].w == w){
      wm->clients[i].mapped = true;
      wm->clients[i].state = ALIVE;
    }
  }

  XMapWindow(wm->d, w);
}

void unmap(struct WM* wm, Window w){
  for(int i = 0; i<100; i++){
    if(wm->clients[i].w == w){
      wm->clients[i].mapped = false;
    }
  }

  XUnmapWindow(wm->d, w);
}
