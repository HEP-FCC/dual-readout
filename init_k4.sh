#!/bin/sh

export WORKDIR=$PWD

if [ ! -d k4FWCore ]; then
  # FIXME LCG100 does not support k4FWCore and HSF spackages do not support ivy-bridge :(
  git clone https://github.com/key4hep/k4FWCore
  cd k4FWCore
  git checkout v01-00pre06
  mkdir build install
  cd build
  cmake -DCMAKE_INSTALL_PREFIX=../install .. && \
  make -j `getconf _NPROCESSORS_ONLN` && \
  make install
  cd $WORKDIR
fi

cd k4FWCore/install

export k4FWCore_DIR=$PWD
export CMAKE_PREFIX_PATH=$CMAKE_PREFIX_PATH:$k4FWCore_DIR/lib/cmake/k4FWCore
export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:$k4FWCore_DIR/lib
export PYTHONPATH=$PYTHONPATH:$k4FWCore_DIR/python
export PATH=$PATH:$k4FWCore_DIR/bin

cd $WORKDIR

if [ ! -d k4SimGeant4 ]; then
  git clone https://github.com/SanghyunKo/k4SimGeant4 # FIXME move to official repo when LCG CMake issue fixed
  cd k4SimGeant4
  git checkout gaudiCMakeFix
  mkdir build install
  cd build
  cmake .. -DCMAKE_INSTALL_PREFIX=../install -DCMAKE_CXX_STANDARD=17
  make -j `getconf _NPROCESSORS_ONLN`
  make install
  cd $WORKDIR
fi

cd k4SimGeant4/install

export k4SimGeant4_DIR=$PWD
export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:$k4SimGeant4_DIR/lib
export PYTHONPATH=$PYTHONPATH:$k4SimGeant4_DIR/python
export CMAKE_PREFIX_PATH=$CMAKE_PREFIX_PATH:$k4SimGeant4_DIR/lib/cmake/k4SimGeant4

cd $WORKDIR
