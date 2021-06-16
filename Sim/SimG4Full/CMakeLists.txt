project(DRSimG4Full)

file(GLOB sources
  ${PROJECT_SOURCE_DIR}/src/*.cpp
)

file(GLOB headers
  ${PROJECT_SOURCE_DIR}/include/*.h
)

add_library(DRSimG4Full SHARED ${sources} ${headers})

target_include_directories(DRSimG4Full
  PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
  $<INSTALL_INTERFACE:include>
)

set_target_properties(DRSimG4Full PROPERTIES PUBLIC_HEADER "${headers}")

target_link_libraries(
  DRSimG4Full
  ${Geant4_LIBRARIES}
  DD4hep::DDCore
  DD4hep::DDG4
  ddDRsegmentation
  DRcomponents
  podio::podioRootIO
  edm4dr
)

install(TARGETS DRSimG4Full EXPORT SimTargets
  LIBRARY DESTINATION "${CMAKE_INSTALL_LIBDIR}" COMPONENT shlib
  PUBLIC_HEADER DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}" COMPONENT dev
)
