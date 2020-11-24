#----------------------------------------------------------------------------
# Setup the project
project(DRsim)

find_package(HepMC3)

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

#----------------------------------------------------------------------------
# Setup Geant4 include directories and compile definitions
# Setup include directory for this project
#
include(${Geant4_USE_FILE})

#----------------------------------------------------------------------------
# Locate sources and headers for this project
# NB: headers are included so they will show up in IDEs
#
include_directories(
  ${HEPMC3_INCLUDE_DIR}
  ${HEPMC3_INTERFACES_DIR}
  ${CMAKE_CURRENT_SOURCE_DIR}/include
  ${Geant4_INCLUDE_DIR}
)
file(GLOB sources ${PROJECT_SOURCE_DIR}/src/*.cc)
file(GLOB headers ${PROJECT_SOURCE_DIR}/include/*.hh)

#----------------------------------------------------------------------------
# Add the executable, and link it to the Geant4 libraries
#
add_executable(DRsim DRsim.cc ${sources} ${headers})
target_link_libraries(
  DRsim ${Geant4_LIBRARIES}
  ${HEPMC3_LIBRARIES}
  ${HEPMC3_ROOTIO_LIB}
  rootIO
  DetComponents
  ddDRcalo
  ddDRsensitive
  SimG4Components
  SimG4Full
  SimG4Fast
)

file(GLOB DRsim_MACROS ${PROJECT_SOURCE_DIR}/*.mac compacts.txt)
file(COPY ${DRsim_MACROS} DESTINATION ${PROJECT_BINARY_DIR})

#----------------------------------------------------------------------------
# Add program to the project targets
# (this avoids the need of typing the program name after make)
#
#add_custom_target(DRsim)

#----------------------------------------------------------------------------
# Install the executable to 'bin' directory under CMAKE_INSTALL_PREFIX
#
install(TARGETS DRsim DESTINATION bin)
install(FILES ${DRsim_MACROS} DESTINATION .)
