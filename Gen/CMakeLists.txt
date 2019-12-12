cmake_minimum_required(VERSION 2.6 FATAL_ERROR)
project(P8ptcgen)

find_package(ROOT REQUIRED)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(HEPMC_INCLUDE_DIR "$ENV{HEPMC_INCLUDE_DIR}")
set(HEPMCROOTIO_INCLUDE_DIR "$ENV{HEPMCROOTIO_INCLUDE_DIR}")
set(HEPMC_LIBRARIES "$ENV{HEPMC_LIBRARIES}")
set(HEPMCROOTIO_LIBRARIES "$ENV{HEPMCROOTIO_LIBRARIES}")

set(PYTHIA_INCLUDE_DIR "$ENV{PYTHIA_INCLUDE_DIR}")
set(PYTHIA_LIBRARIES "$ENV{PYTHIA_LIBRARIES}")

set(FASTJET_INCLUDE_DIR "$ENV{FASTJET_INCLUDE_DIR}")
set(FASTJET_LIBRARIES "$ENV{FASTJET_LIBRARIES}")

set(DR_DIR "$ENV{DR_DIR}")

include_directories(
  ${CMAKE_CURRENT_SOURCE_DIR}/include
  ${PYTHIA_INCLUDE_DIR}
  ${HEPMC_INCLUDE_DIR}
  ${HEPMCROOTIO_INCLUDE_DIR}
  ${FASTJET_INCLUDE_DIR}
  #${ROOT_INCLUDE_DIRS}

  ${DR_DIR}/Common/include
)
file(GLOB sources
  ${PROJECT_SOURCE_DIR}/src/*.cc
  #${DR_DIR}/Common/src/*.cc
)
file(GLOB headers
  ${PROJECT_SOURCE_DIR}/include/*.h
)

include(${ROOT_USE_FILE})
ROOT_GENERATE_DICTIONARY(G__fastjetInterface ${DR_DIR}/Common/include/fastjetInterface.h LINKDEF ${DR_DIR}/Common/include/LinkDef.h)
add_library(fastjetInterface SHARED ${DR_DIR}/Common/src/fastjetInterface.cc G__fastjetInterface.cxx)
target_link_libraries(fastjetInterface ${ROOT_LIBRARIES})

add_executable(P8ptcgen P8ptcgen.cc ${sources} ${headers})
target_link_libraries(
  P8ptcgen
  ${PYTHIA_LIBRARIES}
  ${HEPMC_LIBRARIES}
  ${HEPMCROOTIO_LIBRARIES}
  ${FASTJET_LIBRARIES}
  fastjetInterface
  #${ROOT_Tree_LIBRARY}
  #${ROOT_Core_LIBRARY}
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
