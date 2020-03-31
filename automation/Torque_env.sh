#!/bin/sh

export PATH=/cvmfs/sft.cern.ch/lcg/contrib/CMake/3.14.2/Linux-x86_64/bin:$PATH
source /cvmfs/sft.cern.ch/lcg/contrib/gcc/8/x86_64-slc6/setup.sh

source /cvmfs/sft.cern.ch/lcg/releases/LCG_96b/ROOT/6.18.04/x86_64-slc6-gcc8-opt/ROOT-env.sh
source /cvmfs/sft.cern.ch/lcg/releases/LCG_96b/Geant4/10.05.p01.1/x86_64-slc6-gcc8-opt/bin/geant4.sh

export HEPMC_DIR=/cvmfs/sft.cern.ch/lcg/releases/LCG_96b/hepmc3/3.1.2/x86_64-slc6-gcc8-opt
export FASTJET_DIR=/cvmfs/sft.cern.ch/lcg/releases/LCG_96b/fastjet/3.3.2/x86_64-slc6-gcc8-opt
export PYTHIA_DIR=/cvmfs/sft.cern.ch/lcg/releases/LCG_96b/MCGenerators/pythia8/240/x86_64-slc6-gcc8-opt

export PYTHIA8=/cvmfs/sft.cern.ch/lcg/releases/LCG_96b/MCGenerators/pythia8/240/x86_64-slc6-gcc8-opt
export PYTHIA8DATA=/cvmfs/sft.cern.ch/lcg/releases/LCG_96b/MCGenerators/pythia8/240/x86_64-slc6-gcc8-opt/share/Pythia8/xmldoc
export ROOT_INCLUDE_PATH=/cvmfs/sft.cern.ch/lcg/releases/LCG_96b/ROOT/6.18.04/x86_64-slc6-gcc8-opt/include:$ROOT_INCLUDE_PATH
