#!/bin/sh

export WORKDIR=$PWD

SCRIPT_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )"

cd $SCRIPT_DIR/..

if [ ! -d SimSiPM ]; then
  git clone https://github.com/EdoPro98/SimSiPM
  cd SimSiPM
  git checkout v2.0.2
  mkdir build install
  cd build
  cmake -DCMAKE_INSTALL_PREFIX=../install .. && \
  make -j `getconf _NPROCESSORS_ONLN` && \
  make install
  cd $SCRIPT_DIR/..
fi

cd SimSiPM/install

export SimSiPM_DIR=$PWD
export CMAKE_PREFIX_PATH=$CMAKE_PREFIX_PATH:$SimSiPM_DIR/share/sipm/cmake
export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:$SimSiPM_DIR/lib64

cd $WORKDIR
