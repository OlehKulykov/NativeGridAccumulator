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
mkdir -p /dev/shm/NativeGridAccumulator/
cmake -B /dev/shm/NativeGridAccumulator/ -DCMAKE_BUILD_TYPE=Release -DOPT_DEPS_PATH=/dev/shm/NativeGridAccumulator_deps -DOPT_WITH_SYSTEM_CURL=1 -DOPT_WITH_SYSTEM_SQLITE=1 -DOPT_BUILD_NUM=${CI_NEW_BUILD_NUMBER} .
cd /dev/shm/NativeGridAccumulator/
make -j4
#rc-service ngad stop
#mv ngad /usr/local/bin/
#rc-service ngad start
cd ..
rm -rf NativeGridAccumulator
