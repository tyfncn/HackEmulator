#!/bin/bash

gcc hack.c gfx.c -o TheHackMachine -I/usr/X11R6/include -L/usr/X11R6/lib -lX11 -lm
