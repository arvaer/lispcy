#!/bin/sh
#
set -xe
cc -Wall -Wextra -ansi -pedantic -o main parser.c mpc.c -lm -lreadline
