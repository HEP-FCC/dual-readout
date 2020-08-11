cmake_minimum_required(VERSION 3.8 FATAL_ERROR)
cmake_policy(SET CMP0074 NEW)
project(dual-readout)

IF(NOT TARGET DD4hep::DDCore)
  find_package ( DD4hep REQUIRED )
  include ( ${DD4hep_DIR}/cmake/DD4hep.cmake )
  include ( ${DD4hep_DIR}/cmake/DD4hepBuild.cmake )
  dd4hep_configure_output()
ENDIF()

dd4hep_set_compiler_flags()
#==========================================================================
dd4hep_print("|++> ddDRcalo: ROOT version: ${ROOT_VERSION}")
#--------------------------------------------------------------------------
dd4hep_configure_output()

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(HEPMC_DIR "$ENV{HEPMC_DIR}")
set(PYTHIA_DIR "$ENV{PYTHIA_DIR}")
set(FASTJET_DIR "$ENV{FASTJET_DIR}")
set(BOOST_ROOT "$ENV{BOOST_ROOT}")

add_subdirectory(rootIO)
add_subdirectory(Detector)
add_subdirectory(DRsim)
add_subdirectory(Sim)
add_subdirectory(Gen)
add_subdirectory(Reco)
add_subdirectory(analysis)
