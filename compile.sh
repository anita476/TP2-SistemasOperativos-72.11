#!/bin/bash

if [ "$1" == "BUDDY" ]; then
  MEMORY_MANAGER="BUDDY"
else
  MEMORY_MANAGER="LIST"
fi

make clean
cd Toolchain
make clean
make all
cd ..
make all MEMORY_MANAGER=$MEMORY_MANAGER