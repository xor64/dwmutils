#include "plug.h"
#include <stdio.h>
#include <unistd.h>
plug_t PLUG = {0};

static FILE* fp = NULL;

char* batt_files[] = {
    "/sys/class/power_supply/BAT0/capacity",
    "/sys/class/power_supply/BAT1/capacity"
};

void plug_init(void) {
    PLUG.name = "Battery";
    PLUG.version = "0.0.1";

    for (size_t i = 0; i < sizeof(batt_files)/sizeof(batt_files[0]); i++) {
        if (access(batt_files[i], F_OK) == 0) {
            fp = fopen(batt_files[i], "r");
            if (fp) {
                printf("Opened %s\n", batt_files[i]);
                break;
            }
        }
    }

    if (!fp) {
        printf("MODULE: ERROR: Unable to find battery\n");
        return;
    }
}

void plug_reload(void) { /* Unused */ }

void plug_poll(char* buf, size_t len) {
    int perc = 0;
    fseek(fp, 0, SEEK_SET);
    fscanf(fp, "%d", &perc);
    snprintf(buf, len, "Batt: %d%%", perc);
}

void plug_free(void) {}
