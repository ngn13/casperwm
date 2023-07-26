#include "workspace.h"
#include "layouts.h"
#include "log.h"
#include "event.h"
#include "wm.h"

void add_to_workspace(struct Workspace workspace, struct Client client){
  for ( int i = 0; i < 100; i++ ){
    if(workspace.clients[i].state == DEAD || workspace.clients[i].state == 0){
      workspace.clients[i] = client; 
      break;
    }
  }
}

struct Client* get_from_workspace(struct Workspace workspace, Window w){
  for ( int i = 0; i < 100; i++ ){
    if(workspace.clients[i].w == w){
      return &workspace.clients[i];
    }
  }

  return NULL;
}

void remove_from_workspace(struct Workspace workspace, struct Client client){
  for ( int i = 0; i < 100; i++ ){
    if(workspace.clients[i].w == client.w){
     workspace.clients[i].state = DEAD;
     break;
    }
  }
}

void render(struct WM* wm){
  info("Rendering all windows");
  struct Workspace workspace = wm->workspaces[wm->current_workspace];

  for(int i = 0; i < 100; i++){
    if(!workspace.clients[i].mapped && workspace.clients[i].state == ALIVE){
      map(wm, workspace.clients[i].w);
    }
  }

  call_layout(wm);
}

void unrender(struct WM* wm){
  info("Unrendering all windows");
  struct Workspace workspace = wm->workspaces[wm->current_workspace];

  for(int i = 0; i < 100; i++){
    if(workspace.clients[i].mapped && workspace.clients[i].state == ALIVE){
      XUnmapWindow(wm->d, workspace.clients[i].w);
    }
  }
}
