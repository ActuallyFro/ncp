#!/bin/sh
gcc -o bin/ncp.$(uname -sm | awk '{print $1"."$2}') ncp.c
