project(DRreco)

file(GLOB sources
  ${PROJECT_SOURCE_DIR}/src/*.cpp
)

file(GLOB headers
  ${PROJECT_SOURCE_DIR}/include/*.h
)

gaudi_add_module(DRreco
  SOURCES ${sources}
  LINK
  edm4dr
  DRsegmentation
  Gaudi::GaudiKernel
  k4FWCore::k4FWCore
)

target_include_directories(DRreco PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
  $<INSTALL_INTERFACE:include>
)

set_target_properties(DRreco PROPERTIES PUBLIC_HEADER "${headers}")

file(GLOB scripts ${PROJECT_SOURCE_DIR}/test/*.py)

file(COPY ${scripts} DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/test)
file(COPY calib.csv DESTINATION ${CMAKE_CURRENT_BINARY_DIR})

install(TARGETS DRreco
  EXPORT RecoTargets
  RUNTIME DESTINATION "${CMAKE_INSTALL_BINDIR}" COMPONENT bin
  LIBRARY DESTINATION "${CMAKE_INSTALL_LIBDIR}" COMPONENT shlib
  PUBLIC_HEADER DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}" COMPONENT dev
)

install(FILES ${scripts} DESTINATION test)
install(FILES calib.csv DESTINATION ${CMAKE_INSTALL_DATADIR})
