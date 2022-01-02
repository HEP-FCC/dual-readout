project(DRsimG4Full)

file(GLOB libsrc
  ${PROJECT_SOURCE_DIR}/src/lib/*.cpp
)

file(GLOB libheaders
  ${PROJECT_SOURCE_DIR}/include/*.h
)

gaudi_add_library(DRsimG4Full
  SOURCES ${libsrc}
  LINK
  edm4dr
  ${Geant4_LIBRARIES}
  DRsegmentation
  DD4hep::DDCore
  DD4hep::DDG4
)

set_target_properties(DRsimG4Full PROPERTIES PUBLIC_HEADER "${libheaders}")

file(GLOB compsrc
  ${PROJECT_SOURCE_DIR}/src/components/*.cpp
)

file(GLOB compheaders
  ${PROJECT_SOURCE_DIR}/src/components/*.h
)

gaudi_add_module(DRsimG4FullPlugins
  SOURCES ${compsrc}
  LINK
  Gaudi::GaudiKernel
  k4FWCore::k4FWCore
  ${Geant4_LIBRARIES}
  DRsegmentation
  DRsimG4Full
)

set_target_properties(DRsimG4FullPlugins PROPERTIES PUBLIC_HEADER "${compheaders}")

install(TARGETS DRsimG4Full EXPORT SimTargets
  LIBRARY DESTINATION "${CMAKE_INSTALL_LIBDIR}" COMPONENT shlib
  PUBLIC_HEADER DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}" COMPONENT dev
)

install(TARGETS DRsimG4FullPlugins EXPORT SimTargets
  LIBRARY DESTINATION "${CMAKE_INSTALL_LIBDIR}" COMPONENT shlib
  PUBLIC_HEADER DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}" COMPONENT dev
)
