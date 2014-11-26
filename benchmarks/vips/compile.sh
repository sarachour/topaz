#!/bin/bash

PWD=$(pwd)
cd src;
#make clean;
#autoconf;
#./configure --prefix="$PWD/bin" LDFLAGS="-L$TOPAZ_ROOT/lib" LIBS="-ltopaz" CPPFLAGS="-I$TOPAZ_ROOT/include";
make;
make install;
