#!/bin/sh

source /cvmfs/sft.cern.ch/lcg/views/LCG_100/x86_64-centos7-gcc8-opt/setup.sh
export PYTHIA8_ROOT_DIR=/cvmfs/sft.cern.ch/lcg/views/LCG_100/x86_64-centos7-gcc8-opt/
source /cvmfs/sft.cern.ch/lcg/releases/LCG_100/DD4hep/01.16.01/x86_64-centos7-gcc8-opt/bin/thisdd4hep.sh

# LCG100 has no k4FWCore and k4SimGeant4
# install it manually and do
# export CMAKE_PREFIX_PATH=$CMAKE_PREFIX_PATH:<path_to_k4stack_install>/lib/cmake/k4FWCore
# export PATH=$PATH:<path_to_k4stack_install>/bin
# export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:<path_to_k4stack_install>/lib
# export PYTHONPATH=$PYTHONPATH:<path_to_k4stack_install>/python
# HSF spackages do not work on ivy-bridge architecture

SCRIPT_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )"
source $SCRIPT_DIR/init_k4.sh
source $SCRIPT_DIR/init_sipm.sh
