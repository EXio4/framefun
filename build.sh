#!/bin/bash

mkdir -p build/
g++ -o build/fb fb.cxx -O0 -g -lm
