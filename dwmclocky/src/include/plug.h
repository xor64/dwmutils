
#ifndef _H_PLUG
#define _H_PLUG

#include <stddef.h>

typedef void (*plug_init_f) ();
typedef void (*plug_reload_f) ();
typedef void (*plug_poll_f) (char*, size_t);
typedef void (*plug_free_f) ();

typedef struct plug_t {
    char* name;
    char* version;
} plug_t;

typedef struct plug_int_t {
    plug_init_f f_init;
    plug_reload_f f_reload;
    plug_poll_f f_poll;
    plug_free_f f_free;
    plug_t* s_plug_info;
} plug_int_t;

void setup_plugins(char* plugin_path);
char* poll_plugins();
void free_plugins();

#endif

