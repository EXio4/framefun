#!/bin/bash

mkdir -p build/

mkdir -p font/

if [[ "$1" != "ignore_fonts" ]]; then

    rm -f font/font.h
    for f in {a..z} {A..Z} {0..9}; do
        convert -resize 16x32\! -font ./letvezi.ttf -pointsize 14 label:"$f" "$f".xbm
        cat "$f".xbm | tail --lines=7 | sed 's/^static char \(.*\)_bits\[\]/bits['"'\1'"']/' >> font/font.h
        rm "$f".xbm
    done

fi

g++ -o build/fb -Iinclude/ -Ifont/ main.cxx src/* -O0 -g -lm -static -std=c++11
