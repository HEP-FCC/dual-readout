project(DRsensitive)

file(GLOB sources
  ${PROJECT_SOURCE_DIR}/src/*.cpp
)

file(GLOB headers
  ${PROJECT_SOURCE_DIR}/include/*.h
)

add_library(DRsensitive SHARED ${sources} ${headers})

target_include_directories(DRsensitive PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
  $<INSTALL_INTERFACE:include>
)

set_target_properties(DRsensitive PROPERTIES PUBLIC_HEADER "${headers}")

target_link_libraries(
  DRsensitive
  DRsegmentation
  DD4hep::DDCore
  DD4hep::DDG4
)

dd4hep_generate_rootmap(DRsensitive)

install(TARGETS DRsensitive EXPORT DetectorTargets
  LIBRARY DESTINATION "${CMAKE_INSTALL_LIBDIR}" COMPONENT shlib
  PUBLIC_HEADER DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}" COMPONENT dev
)
