#!/bin/sh

set -xe

cc -Wall -Wextra -pedantic -Wconversion -Wunreachable-code \
   -Wno-gnu main.c -o ./calcu -lncurses
