# casperwm | a simple x11 window manager

### note
this simple window manager has only what i need, mainly because
i made this thing for myself so **there is not a lot of 
features or customizability**

### features
- vertical split 
- autostart
- workspaces
- some keybindings (open terminal/kill windows/switch workspaces)
- and **nothing else**

### config
hope you know a bit of c, otherwise you won't get to far with this 
wm

for a simple config, create two file in `src` directory: `config.h` and `config.c`
here is a simple `config.h` that you can copy paste:
```c
#ifndef CONFIG_H
#define CONFIG_H
// #############################################

// YOUR CONFIGURATION HERE!!
#define WORKSPACE_COUNT 8 // this will get you 8 seperate workspaces 

#define MARGIN_TOP 50 // top margin, to give your BAR some space
// if your bar is on the bottom set to this to a negative number

#define MARGIN 25 // the window margins, does not effect BAR

#define MOD Mod4Mask // the key that have a logo on it, 
// also known as the "windows" key this will be your master key,
// you will use it as a prefix before all the other bindings

#define BAR "polybar" // name of your bar program, you can check 
// its name with xprop

#define TERMINAL "kitty" // terminal command 

#define LAUNCHER "dmenu_run" // launcher command

void startup(); // just a definition, see config.c

// #############################################
#endif
```
and here is the `config.c`:
```c
#include "config.h"
#include <stdlib.h>
 
// startup will be when the window manager starts,
// wm will fork this function and it will run in 
// the background
void startup(){  
  // here you can startup your bar, your wallpaper browser etc.
  system("nitrogen --restore &");
  system("polybar");
}
```

### compile
after adding your config files you, can compile casperwm by running:
```bash
make
```
note that on debian based systems you will need `libx11-dev` in order to
compile

### install
following command will copy the build to `/usr/bin` and it will add an entry to the
`/usr/share/xsessions` so you can launch the wm from your display manager:
```
sudo make install
```

### advanced stuff
feel free to play around with the code and break things, here is simple breakdown of all the files:
- `capser.c`: contains the main function, waits for XEvents and calls XEvent handler functions from `event.c`
- `event.c`: has all the handler functions, these functions handle different XEvents such as `MapRequest`, `CreateNotify` etc. Also contains the `resize` function that resisez all the windows when called
- `log.c`: a simple wrapper around `printf`, provides the logging functions used in `event.c` and `casper.c`
- `wm.h`: Contains definitions for the `WM` and the `Client` struct

Note that **I am not an expert Xlib developer**, I learned xlib along the way, so this code may look bad to an
expert xlib dev (and it probably is)
