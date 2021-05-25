project(Reco)

include_directories(
  ${CMAKE_CURRENT_SOURCE_DIR}/include
)

file(GLOB sources
  ${PROJECT_SOURCE_DIR}/src/*.cc
)

file(GLOB headers
  ${PROJECT_SOURCE_DIR}/include/*.h
)

# FIXME Exclude RecoTower temporarily
LIST(FILTER headers EXCLUDE REGEX RecoTower.h)
LIST(FILTER sources EXCLUDE REGEX RecoTower.cc)
LIST(FILTER headers EXCLUDE REGEX RecoStandalone.h)
LIST(FILTER sources EXCLUDE REGEX RecoStandalone.cc)

gaudi_add_module(DRreco
  SOURCES ${sources}
  LINK
  edm4dr
  edm4dr::edm4drDict
  ddDRsegmentation
  Gaudi::GaudiKernel
  Gaudi::GaudiAlgLib
  k4FWCore::k4FWCore
  k4FWCore::k4FWCorePlugins
  k4FWCore::k4Interface
)

add_executable(DRrecoExe Reco.cc ${PROJECT_SOURCE_DIR}/src/RecoStandalone.cc ${PROJECT_SOURCE_DIR}/include/RecoStandalone.h)

target_link_libraries(DRrecoExe
  DRcomponents
  ddDRsegmentation
  podio::podioRootIO
  edm4dr
)

set(
  Reco_SCRIPTS
  calib.csv
  test/runDRcaloReco.py
)

foreach(_script ${Reco_SCRIPTS})
  configure_file(
    ${PROJECT_SOURCE_DIR}/${_script}
    ${PROJECT_BINARY_DIR}/${_script}
    COPYONLY
  )
endforeach()

install(TARGETS DRreco
  EXPORT DRcaloRecoTargets
  RUNTIME DESTINATION "${CMAKE_INSTALL_BINDIR}" COMPONENT bin
  LIBRARY DESTINATION "${CMAKE_INSTALL_LIBDIR}" COMPONENT shlib
  PUBLIC_HEADER DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}"
  COMPONENT dev
)

install(TARGETS DRrecoExe
  RUNTIME DESTINATION "${CMAKE_INSTALL_BINDIR}" COMPONENT bin
  PUBLIC_HEADER DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}" COMPONENT dev
)

install(FILES ${PROJECT_BINARY_DIR}/calib.csv DESTINATION .)
install(FILES ${PROJECT_BINARY_DIR}/test/runDRcaloReco.py DESTINATION test)
