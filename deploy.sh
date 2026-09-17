#!/bin/bash

CI_FILE=".build_number.ci"
if [ -f "$CI_FILE" ]; then
  CI_BUILD_NUMBER=$(cat "$CI_FILE")
else
  CI_BUILD_NUMBER=0
fi

CI_NEW_BUILD_NUMBER=$((CI_BUILD_NUMBER + 1))
echo "$CI_NEW_BUILD_NUMBER" > "$CI_FILE"

git pull
mkdir -p /dev/shm/nga/
cmake -B /dev/shm/nga/ -DCMAKE_BUILD_TYPE=Release -DOPT_DEPS_PATH=/dev/shm/nga_deps -DOPT_BUILD_NUM=${CI_NEW_BUILD_NUMBER} .
cd /dev/shm/nga/
make -j4
rc-service nga stop
mv nga-d /usr/local/bin/
mv nga-u /usr/local/bin/
rc-service nga start
cd ..
rm -rf nga
