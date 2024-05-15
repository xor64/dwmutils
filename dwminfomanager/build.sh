#!/usr/bin/bash

set -xe

CCARGS="-Isrc/include -Wall -pedantic"

cc -o dim src/main.c src/plug.c $CCARGS -lX11 -ldl
cc -o modules/clocky.dim src/modules/clock.c -rdynamic -shared -fPIC $CCARGS
cc -o modules/battery.dim src/modules/battery.c -rdynamic -shared -fPIC $CCARGS


