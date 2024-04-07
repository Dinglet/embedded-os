#!/bin/sh

set -x #prints each command and its arguments to the terminal before executing it
# set -e #Exit immediately if a command exits with a non-zero status

rmmod -f mydev
insmod mydev.ko

./writer STUNAME & #run in subshell
./reader 127.0.0.1 8000 /dev/mydev
