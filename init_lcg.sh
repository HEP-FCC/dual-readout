#!/bin/sh

source /cvmfs/sft.cern.ch/lcg/views/LCG_100/x86_64-centos7-gcc8-opt/setup.sh
export PYTHIA8_ROOT_DIR=/cvmfs/sft.cern.ch/lcg/views/LCG_100/x86_64-centos7-gcc8-opt/
source /cvmfs/sft.cern.ch/lcg/releases/LCG_100/hepmc3/3.2.3/x86_64-centos7-gcc8-opt/hepmc3-env.sh
source /cvmfs/sft.cern.ch/lcg/releases/LCG_100/DD4hep/01.16.01/x86_64-centos7-gcc8-opt/bin/thisdd4hep.sh

# LCG100 has no k4FWCore
# install it manually and do 'export CMAKE_PREFIX_PATH=$CMAKE_PREFIX_PATH:<path_to_k4FWCore_install>/lib/cmake/k4FWCore'
# HSF spackages do not work on ivy-bridge architecture
