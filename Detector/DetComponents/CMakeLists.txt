project(DetComponents)

cmake_minimum_required(VERSION 3.12 FATAL_ERROR)

find_package(Geant4 REQUIRED)

include(${Geant4_USE_FILE})

include_directories(
  ${CMAKE_CURRENT_SOURCE_DIR}/include
)

file(GLOB sources
  ${PROJECT_SOURCE_DIR}/src/*.cpp
)

add_library(DetComponents SHARED ${sources})
target_include_directories(DetComponents PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include ${DD4hep_DIR}/include)
target_link_libraries(
  DetComponents
  ${Geant4_LIBRARIES}
  DD4hep::DDCore
  DD4hep::DDG4
)

install(TARGETS DetComponents LIBRARY DESTINATION lib)
