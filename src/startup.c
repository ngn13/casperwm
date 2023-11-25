#include "log.h"
#include "util.h"
#include <unistd.h>

void run_bar() {
  if(strlen(cfg.bar)==0) {
    return print("STARTUP", "Bar not set");
  }

  if(fork()==0) {
    setsid();
    daemon(1,0);
    execvp("polybar", NULL);
    exit(0);
  }
}

void run_script() {
  char* homedir = get_homedir();
  char start_script[strlen(homedir)+200];
  join(homedir, ".config/casperwm/start", start_script);

  if(!exists(start_script)) {
    print("STARTUP", "Startup script not found");
  }

  if(fork()==0){
    setsid();
    daemon(1,0);
    execv(start_script, NULL);
    exit(0);
  }
}

void run_startup() {
  run_script();
  run_bar();
}
