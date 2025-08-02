#!/bin/bash

mkdir -p build

cd build

rm * -rf

stdbuf -oL cmake --preset=default .. |& tee cmake.log

stdbuf -oL cmake --build . |& tee build.log