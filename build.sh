#!/bin/bash

g++ -std=c++11 -o build.run build.cxx -O1 && ./build.run $@
