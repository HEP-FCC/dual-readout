#!/bin/sh

source /cvmfs/sft.cern.ch/lcg/views/LCG_102rc1/x86_64-centos7-gcc11-opt/setup.sh
source /cvmfs/sft.cern.ch/lcg/releases/LCG_102rc1/DD4hep/01.20.1/x86_64-centos7-gcc11-opt/bin/thisdd4hep.sh

# LCG102 has no k4FWCore and k4SimGeant4
# install it manually and do
# export CMAKE_PREFIX_PATH=$CMAKE_PREFIX_PATH:<path_to_k4stack_install>/lib/cmake/k4FWCore
# export PATH=$PATH:<path_to_k4stack_install>/bin
# export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:<path_to_k4stack_install>/lib
# export PYTHONPATH=$PYTHONPATH:<path_to_k4stack_install>/python
# HSF spackages do not work on ivy-bridge architecture

SCRIPT_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )"
source $SCRIPT_DIR/init_k4.sh
source $SCRIPT_DIR/init_sipm.sh
