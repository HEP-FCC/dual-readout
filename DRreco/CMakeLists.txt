project(DRreco)

file(GLOB sources
  ${PROJECT_SOURCE_DIR}/src/*.cpp
)

file(GLOB headers
  ${PROJECT_SOURCE_DIR}/include/*.h
)

find_package(CLHEP)

gaudi_add_module(DRreco
  SOURCES ${sources}
  LINK
  EDM4HEP::edm4hep
  DRsegmentation
  Gaudi::GaudiKernel
  k4FWCore::k4FWCore
  CLHEP::CLHEP
)

target_include_directories(DRreco PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
  $<INSTALL_INTERFACE:include>
)

set_target_properties(DRreco PROPERTIES PUBLIC_HEADER "${headers}")

file(GLOB scripts ${PROJECT_SOURCE_DIR}/test/*.py)
file(GLOB datafiles ${PROJECT_SOURCE_DIR}/data/*)

file(COPY ${scripts} DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/test)
file(COPY ${datafiles} DESTINATION ${CMAKE_CURRENT_BINARY_DIR})

install(TARGETS DRreco
  EXPORT RecoTargets
  RUNTIME DESTINATION "${CMAKE_INSTALL_BINDIR}" COMPONENT bin
  LIBRARY DESTINATION "${CMAKE_INSTALL_LIBDIR}" COMPONENT shlib
  PUBLIC_HEADER DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}" COMPONENT dev
)

install(FILES ${scripts} DESTINATION test)
install(FILES ${datafiles} DESTINATION ${CMAKE_INSTALL_DATADIR})
