#!/bin/sh

export WORKDIR=$PWD

SCRIPT_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )"

cd $SCRIPT_DIR/.. #FIXME libReco.so requires k4FWCore library to coincide with ../../k4FWCore/install/lib/libk4FWCorePlugins.so 

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
  cd $SCRIPT_DIR/..
fi

cd k4FWCore/install

export k4FWCore_DIR=$PWD
export CMAKE_PREFIX_PATH=$CMAKE_PREFIX_PATH:$k4FWCore_DIR/lib/cmake/k4FWCore
export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:$k4FWCore_DIR/lib
export PYTHONPATH=$PYTHONPATH:$k4FWCore_DIR/python
export PATH=$PATH:$k4FWCore_DIR/bin

cd $SCRIPT_DIR/..

if [ ! -d k4SimGeant4 ]; then
  git clone https://github.com/HEP-FCC/k4SimGeant4
  cd k4SimGeant4
  git checkout bb238a170a1a73bc5cffc5a861fd6a60f412cfb0
  mkdir build install
  cd build
  cmake .. -DCMAKE_INSTALL_PREFIX=../install -DCMAKE_CXX_STANDARD=17
  make -j `getconf _NPROCESSORS_ONLN`
  make install
  cd $SCRIPT_DIR/..
fi

cd k4SimGeant4/install

export k4SimGeant4_DIR=$PWD
export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:$k4SimGeant4_DIR/lib
export PYTHONPATH=$PYTHONPATH:$k4SimGeant4_DIR/python
export CMAKE_PREFIX_PATH=$CMAKE_PREFIX_PATH:$k4SimGeant4_DIR/lib/cmake/k4SimGeant4

cd $WORKDIR
