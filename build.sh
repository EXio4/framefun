#!/bin/bash
if ! [[ -x ./build.run ]]; then
   echo "[.. compiling building manager ..]"
   g++ -std=c++11 -o build.run build.cxx -O1
fi

exec ./build.run $@
