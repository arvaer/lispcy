#!/bin/sh
#
set -xe
cc -Wall -Wextra -ansi -pedantic -o main main.c mpc.c -lm -lreadline
