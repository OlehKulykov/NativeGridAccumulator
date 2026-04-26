#!/bin/bash

git pull
mkdir -p /dev/shm/NativeGrid/
cmake -B /dev/shm/NativeGrid/ -DCMAKE_BUILD_TYPE=Release .
cd /dev/shm/NativeGrid/
make -j4
rc-service nativegrid stop
mv nativegrid /usr/local/bin/
rc-service nativegrid start
cd ..
rm -rf NativeGrid
