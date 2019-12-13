cmake_minimum_required(VERSION 2.6 FATAL_ERROR)
project(P8ptcgen)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(HEPMC_DIR "$ENV{HEPMC_DIR}")
set(PYTHIA_DIR "$ENV{PYTHIA_DIR}")
set(FASTJET_DIR "$ENV{FASTJET_DIR}")

include_directories(
  ${CMAKE_CURRENT_SOURCE_DIR}/include
  ${PYTHIA_DIR}/include
  ${HEPMC_DIR}/include
  ${FASTJET_DIR}/include
)
file(GLOB sources
  ${PROJECT_SOURCE_DIR}/src/*.cc
)
file(GLOB headers
  ${PROJECT_SOURCE_DIR}/include/*.h
)

add_executable(P8ptcgen P8ptcgen.cc ${sources} ${headers})
target_link_libraries(
  P8ptcgen
  ${PYTHIA_DIR}/lib/libpythia8.a
  ${HEPMC_DIR}/lib/libHepMC3.so
  ${HEPMC_DIR}/lib/libHepMC3rootIO.so
  ${FASTJET_DIR}/lib/libfastjet.so
  rootIO
  ${CMAKE_DL_LIBS}
)

set(
  P8ptcgen_SCRIPTS
  ptcgun.cmnd
)

foreach(_script ${P8ptcgen_SCRIPTS})
  configure_file(
    ${PROJECT_SOURCE_DIR}/${_script}
    ${PROJECT_BINARY_DIR}/${_script}
    COPYONLY
  )
endforeach()

install(TARGETS P8ptcgen DESTINATION bin)
