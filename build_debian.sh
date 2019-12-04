#!/bin/bash

set -e
set -x

mkdir -p build
cd qradiolink/ext/
protoc --cpp_out=. Mumble.proto
protoc --cpp_out=. QRadioLink.proto
cd ../../build/
qmake "CONFIG+=debug" ..
make -j3
cp -p ./qradiolink ../
cd ..
./qradiolink
