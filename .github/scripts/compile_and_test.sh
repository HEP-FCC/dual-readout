#!/bin/bash

export WORKDIR=$PWD

# FIXME LCG100 does not support k4FWCore and HSF spackages do not support ivy-bridge :( 
git clone https://github.com/key4hep/k4FWCore
cd k4FWCore
git checkout v01-00pre06
mkdir build install
cd build
cmake -DCMAKE_INSTALL_PREFIX=../install .. && \
make -j `getconf _NPROCESSORS_ONLN` && \
make install
cd ../install
export k4FWCore_DIR=$PWD
export CMAKE_PREFIX_PATH=$CMAKE_PREFIX_PATH:$k4FWCore_DIR/lib/cmake/k4FWCore

cd $WORKDIR

mkdir build install
cd build
cmake -DCMAKE_INSTALL_PREFIX=../install -DCMAKE_CXX_STANDARD=17  .. && \
make  -j `getconf _NPROCESSORS_ONLN` && \
make install && \
ls -alhR $G4LEDATA && \
ctest -j `getconf _NPROCESSORS_ONLN` --output-on-failure
