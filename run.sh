#!/bin/bash -e

# Tested on OpenCV 2.4.13 and Ubuntu 14.04
g++ code.cpp -o code `pkg-config --cflags --libs opencv` 

PATTERN=$1
VIDEO=$2
./code $PATTERN $VIDEO
