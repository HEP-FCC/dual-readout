cmake_minimum_required(VERSION 3.12 FATAL_ERROR)
project(dual-readout LANGUAGES CXX)

set(CMAKE_MODULE_PATH
    ${CMAKE_MODULE_PATH}
    ${CMAKE_CURRENT_SOURCE_DIR}/cmake)

# Use GNU-style hierarchy for installing build products
include(GNUInstallDirs)
include(cmake/Key4hepConfig.cmake)

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

find_package ( DD4hep REQUIRED )
dd4hep_set_compiler_flags()
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR}/bin)


find_package(ROOT REQUIRED)
find_package(podio REQUIRED HINTS $ENV{PODIO})
find_package(Gaudi REQUIRED)
find_package(EDM4HEP)
find_package(k4FWCore 1.3 REQUIRED)
find_package(sipm REQUIRED)

set(BOOST_ROOT "$ENV{BOOST_ROOT}")

add_subdirectory(Detector)
add_subdirectory(DRsim)
add_subdirectory(DRdigi)
add_subdirectory(DRreco)
add_subdirectory(analysis)
