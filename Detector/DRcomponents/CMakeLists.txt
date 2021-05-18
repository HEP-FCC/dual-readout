project(DRcomponents)

include_directories(
  ${CMAKE_CURRENT_SOURCE_DIR}/include
)

file(GLOB sources
  ${PROJECT_SOURCE_DIR}/src/*.cpp
)

file(GLOB headers
  ${PROJECT_SOURCE_DIR}/include/*.h
)

add_library(DRcomponents SHARED ${sources} ${headers})

target_include_directories(DRcomponents PUBLIC
  ${CMAKE_CURRENT_SOURCE_DIR}/include
  ${DD4hep_DIR}/include
)

target_link_libraries(
  DRcomponents
  DD4hep::DDCore
  DD4hep::DDG4
)

install(TARGETS DRcomponents
  LIBRARY DESTINATION "${CMAKE_INSTALL_LIBDIR}" COMPONENT shlib
  PUBLIC_HEADER DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}" COMPONENT dev
)
