cmake_minimum_required(VERSION 3.8 FATAL_ERROR)
cmake_policy(SET CMP0074 NEW)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
project(dual-readout)

set(CMAKE_MODULE_PATH
    ${CMAKE_MODULE_PATH}
    ${CMAKE_CURRENT_SOURCE_DIR}/cmake)

# Use GNU-style hierarchy for installing build products
include(GNUInstallDirs)

find_package(ROOT REQUIRED)
find_package(HepMC3) # FIXME setting REQUIRED results Gaudi bug
find_package(podio REQUIRED HINTS $ENV{PODIO})
find_package(FastJet)
find_package(Gaudi REQUIRED)
find_package(k4FWCore REQUIRED)
find_package(k4SimGeant4 REQUIRED)
find_package(sipm REQUIRED)

#----------------------------------------------------------------------------
# Find Geant4 package, activating all available UI and Vis drivers by default
# You can set WITH_GEANT4_UIVIS to OFF via the command line or ccmake/cmake-gui
# to build a batch mode only executable
#
option(WITH_GEANT4_UIVIS "Build example with Geant4 UI and Vis drivers" OFF)
if(WITH_GEANT4_UIVIS)
  find_package(Geant4 REQUIRED ui_all vis_all)
else()
  find_package(Geant4 REQUIRED)
endif()

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

set(BOOST_ROOT "$ENV{BOOST_ROOT}")

add_subdirectory(edm4dr)
add_subdirectory(Detector)
#add_subdirectory(DRsim)
add_subdirectory(Sim)
add_subdirectory(DRdigi)
#add_subdirectory(Gen)
add_subdirectory(DRreco)
add_subdirectory(analysis)
