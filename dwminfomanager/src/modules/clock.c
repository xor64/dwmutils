#include "plug.h"
#include <stdio.h>
#include <time.h>
plug_t PLUG = {0};

void plug_init(void) {
    PLUG.name = "time";
    PLUG.version = "0.0.1";
}

void plug_reload(void) { /* Unused */ }

void plug_poll(char* buf, size_t len) {
    time_t rawtime;
    struct tm *timeinfo;
    
    time(&rawtime);
    timeinfo = localtime(&rawtime);

    strftime(buf, len, "Time: %H:%M (%Y-%m-%d)", timeinfo);
}

void plug_free(void) {}
