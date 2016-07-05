#!/bin/bash

export PATH=$PATH:/usr/xenomai/bin
export CFLAGS="$(xeno-config --skin=native --cflags)"
export LDFLAGS="$(xeno-config --skin=native --ldflags)"
export LD_LIBRARY_PATH=/usr/xenomai/lib

gcc $CFLAGS $LDFLAGS -lm lib/*.c server/server.c -I. -L./lib -o bin/server
