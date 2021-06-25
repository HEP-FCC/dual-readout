project(DRSimG4Fast)

file(GLOB sources
  ${PROJECT_SOURCE_DIR}/src/*.cpp
)

file(GLOB headers
  ${PROJECT_SOURCE_DIR}/include/*.h
)

gaudi_add_module(DRSimG4Fast
  SOURCES ${sources}
  LINK
  Gaudi::GaudiKernel
  k4FWCore::k4FWCore
  ${Geant4_LIBRARIES}
)

target_include_directories(DRSimG4Fast PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
  $<INSTALL_INTERFACE:include>
)

set_target_properties(DRSimG4Fast PROPERTIES PUBLIC_HEADER "${headers}")

install(TARGETS DRSimG4Fast
  LIBRARY DESTINATION "${CMAKE_INSTALL_LIBDIR}" COMPONENT shlib
  PUBLIC_HEADER DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}" COMPONENT dev
)
