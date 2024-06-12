#!/bin/sh

# Copyright (c) 2024 RTES Lab, ECE, National Yang Ming Chiao Tung University
#
# All rights reserved.

set -x #prints each command and its arguments to the terminal before executing it
# set -e #Exit immediately if a command exits with a non-zero status

rmmod -f mydev
insmod mydev.ko

./writer "HUANG, SHIEN-TING" & #run in subshell
./reader 192.168.3.1 8000 /dev/mydev
