#include <stdlib.h>
#include <stdio.h>
#include <dirent.h>
#include <string.h>
#include <dlfcn.h>
#include "plug.h"


#define PLUG_PATH_MAX 255
#define PLUG_MAX_COUNT (PLUG_PATH_MAX+255)
#define PLUG_POLL_BUF_SZ 1024

typedef struct PlugMan {
    plug_int_t* plugs[PLUG_MAX_COUNT];
    size_t plug_count;
    void* plug_objs[PLUG_MAX_COUNT];
    size_t plug_obj_count;
} plugman_t;


static plugman_t PLUGMAN = {0};

void* load_plug_sym(void* plug_f, char* fn_name);
plug_int_t* load_plugin(char* path);

void load_plugins(const char* mod_dir_p) {
    DIR* dir = {0};

    dir = opendir(mod_dir_p);

    if (!dir) {
        printf("ERROR: Failed to open dir %s\n", mod_dir_p);
    }

    struct dirent* dir_entry;

    while ((dir_entry = readdir(dir)) != NULL) {
        if (!(dir_entry->d_type == DT_REG ||
                dir_entry->d_type == DT_LNK)) {
            continue;
        }

        if (strcmp(dir_entry->d_name, ".") == 0 ||
                strcmp(dir_entry->d_name, "..") == 0) {
            continue;
        }

        char* dot = strchr(dir_entry->d_name, '.');
        
        if (!dot || dot == dir_entry->d_name) {
            continue;
        }

        char* ext = dot+1;
        if (strcmp(ext, "dim") != 0) continue;
        
        printf("INFO: Found plugin: %s/%s\n", mod_dir_p, dir_entry->d_name);

        char full_path[PLUG_MAX_COUNT] = {0};
        snprintf(full_path, PLUG_MAX_COUNT, "%s/%s", mod_dir_p, dir_entry->d_name);
        plug_int_t* plug = load_plugin(full_path);

        if (!plug) {
            printf("ERROR: Failed to load plugin %s, skipping\n", full_path);
            continue;
        }
        PLUGMAN.plugs[PLUGMAN.plug_count++] = plug;
    };
}


plug_int_t* load_plugin(char* path) {
    plug_int_t* plug = malloc(sizeof(plug_int_t));
    void* plug_f = dlopen(path, RTLD_NOW);
    
    if (!plug_f) {
        printf("ERROR: Failed to load plugin %s\n", path);
        return NULL;
    }
    
    // "technically" couldnt run on very niche platforms but idc
    #pragma GCC diagnostic push
    #pragma GCC diagnostic ignored "-Wpedantic"
    
    plug->f_init = (plug_init_f)load_plug_sym(plug_f, "plug_init");
    plug->f_reload = (plug_reload_f)load_plug_sym(plug_f, "plug_reload");
    plug->f_poll = (plug_poll_f)load_plug_sym(plug_f, "plug_poll");
    plug->f_free = (plug_free_f)load_plug_sym(plug_f, "plug_free");
    plug->s_plug_info = (plug_t*)load_plug_sym(plug_f, "PLUG");

    #pragma GCC diagnostic pop

    if (!plug->f_init || !plug->f_reload ||
            !plug->f_poll || !plug->f_free ||
            !plug->s_plug_info) {
        dlclose(plug_f);
        return NULL;
    }

    PLUGMAN.plug_objs[PLUGMAN.plug_obj_count++] = plug_f;
    return plug;
}


void* load_plug_sym(void* plug_f, char* fn_name) {
    void* f = dlsym(plug_f, fn_name);
    char* err = dlerror();

    if (err){
       printf("ERROR: Could not load plug symbol '%s': %s\n", fn_name, err);
       return NULL;
    } 
    return f;
}

void setup_plugins(char* plugin_path) {
    load_plugins(plugin_path);
    printf("INFO: Loaded %zu plugins:\n", PLUGMAN.plug_count);
    for (size_t i = 0; i < PLUGMAN.plug_count; i++) {
        plug_int_t* plug = PLUGMAN.plugs[i]; 

        (plug->f_init)();
        printf(" - %s (%s)\n", plug->s_plug_info->name, plug->s_plug_info->version);
    }
}

void free_plugins() {
    for (size_t i = 0; i < PLUGMAN.plug_count; i++) {
        plug_int_t* plug = PLUGMAN.plugs[i];
        (plug->f_free)();
    }

    for (size_t i = 0; i < PLUGMAN.plug_obj_count; i++) {
        dlclose(PLUGMAN.plug_objs[i]);
    }
}



char* poll_plugins(char* sep) {
    char* buf = malloc(PLUG_POLL_BUF_SZ * sizeof(char));

    if (!buf) {
        printf("Unable to allocate buffer for polling\n");
        return NULL;
    }

    for (size_t i = 0; i < PLUGMAN.plug_count; i++) {
        plug_int_t* plug = PLUGMAN.plugs[i];
        
        printf("Polling plug id %zu\n", i);

        size_t len = strlen(buf);
        (plug->f_poll)(buf + len, PLUG_POLL_BUF_SZ - len);
        
        if (i < PLUGMAN.plug_count - 1) {
            size_t len_post = strlen(buf);
            snprintf(buf + len_post, PLUG_POLL_BUF_SZ - len_post, " %s ", sep);
        }
    }

    printf("Setting title: '%s'\n", buf);

    return buf;
}



