project(Reco)

include_directories(
  ${CMAKE_CURRENT_SOURCE_DIR}/include
  ${FASTJET_INCLUDE_DIR}
)

file(GLOB sources
  ${PROJECT_SOURCE_DIR}/src/*.cc
)

file(GLOB headers
  ${PROJECT_SOURCE_DIR}/include/*.h
)

gaudi_add_module(Reco
  SOURCES ${sources}
  LINK
  Gaudi::GaudiKernel
  Gaudi::GaudiAlgLib
  k4FWCore::k4FWCore
  k4FWCore::k4FWCorePlugins
  edm4dr
  edm4dr::edm4drDict
  DetComponents
  ddDRsegmentation
)

add_library( DRcalo::Reco ALIAS Reco )

# TODO is executable needed?
#add_executable(Reco Reco.cc ${sources} ${headers})
#target_link_libraries(
#  Reco
#  ${FASTJET_LIBRARY}
#  DetComponents
#  ddDRsegmentation
#)

set(
  Reco_SCRIPTS
  calib.csv
)

foreach(_script ${Reco_SCRIPTS})
  configure_file(
    ${PROJECT_SOURCE_DIR}/${_script}
    ${PROJECT_BINARY_DIR}/${_script}
    COPYONLY
  )
endforeach()

#install(TARGETS Reco DESTINATION bin)
install(FILES ${PROJECT_BINARY_DIR}/calib.csv DESTINATION .)
