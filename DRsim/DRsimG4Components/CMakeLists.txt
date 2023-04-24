project(DRsimG4Components)

file(GLOB sources
  ${PROJECT_SOURCE_DIR}/src/*.cpp
)

file(GLOB headers
  ${PROJECT_SOURCE_DIR}/include/*.h
)

gaudi_add_module(DRsimG4Components
  SOURCES ${sources}
  LINK
  Gaudi::GaudiKernel
  k4FWCore::k4FWCore
  EDM4HEP::edm4hep
  DRsensitive
  DRsimG4Full
)

target_include_directories(DRsimG4Components
  PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
  $<INSTALL_INTERFACE:include>
)

set_target_properties(DRsimG4Components PROPERTIES PUBLIC_HEADER "${headers}")

install(TARGETS DRsimG4Components EXPORT SimTargets
  LIBRARY DESTINATION "${CMAKE_INSTALL_LIBDIR}" COMPONENT shlib
  PUBLIC_HEADER DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}" COMPONENT dev
)

file(GLOB scripts ${PROJECT_SOURCE_DIR}/test/*.py)
file(COPY ${scripts} DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/test)

install(FILES ${scripts} DESTINATION test)
