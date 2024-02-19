#!/bin/sh

make clean && make
ar rcs lcfextd.a lib.o ih_vec.o
