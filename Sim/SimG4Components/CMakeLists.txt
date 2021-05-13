project(SimG4Components)

cmake_minimum_required(VERSION 3.12 FATAL_ERROR)

include(${Geant4_USE_FILE})

include_directories(
  ${CMAKE_CURRENT_SOURCE_DIR}/include
)

file(GLOB sources
  ${PROJECT_SOURCE_DIR}/src/*.cpp
)

add_library(SimG4Components SHARED ${sources})
target_include_directories(
  SimG4Components
  PUBLIC
  ${CMAKE_CURRENT_SOURCE_DIR}/include
  ${DD4hep_DIR}/include
)

target_link_libraries(
  SimG4Components
  PUBLIC
  ${Geant4_LIBRARIES}
  DD4hep::DDCore
  DD4hep::DDG4
  ddDRsensitive
  ddDRsegmentation
  DetComponents
  podio::podioRootIO
  edm4dr
)

install(TARGETS SimG4Components
  LIBRARY DESTINATION "${CMAKE_INSTALL_LIBDIR}" COMPONENT shlib
  PUBLIC_HEADER DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}" COMPONENT dev
)
