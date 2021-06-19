project(DRSimG4Fast)

file(GLOB sources
  ${PROJECT_SOURCE_DIR}/src/*.cpp
)

file(GLOB headers
  ${PROJECT_SOURCE_DIR}/include/*.h
)

add_library(DRSimG4Fast SHARED ${sources} ${headers})

target_include_directories(DRSimG4Fast PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
  $<INSTALL_INTERFACE:include>
)

set_target_properties(DRSimG4Fast PROPERTIES PUBLIC_HEADER "${headers}")

target_link_libraries(
  DRSimG4Fast
  ${Geant4_LIBRARIES}
  DD4hep::DDCore
  DD4hep::DDG4
)

install(TARGETS DRSimG4Fast
  LIBRARY DESTINATION "${CMAKE_INSTALL_LIBDIR}" COMPONENT shlib
  PUBLIC_HEADER DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}" COMPONENT dev
)
