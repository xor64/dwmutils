#include <X11/Xlib.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include "plug.h"

int main(void) {
    setup_plugins("./modules");
    char* buffer;

    Display *display = XOpenDisplay(NULL);
    Window window = DefaultRootWindow(display);

    while (1) {
        buffer = poll_plugins("|");
        XStoreName(display, window, buffer);
        XFlush(display);
        sleep(1);
    }


  XCloseDisplay(display);
  free_plugins();
  return 0;

}
