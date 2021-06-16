project(Digi)

file(GLOB sources
  ${PROJECT_SOURCE_DIR}/src/*.cc
)

file(GLOB headers
  ${PROJECT_SOURCE_DIR}/include/*.h
)

add_executable(DRdigi Digi.cc ${sources} ${headers})

target_include_directories(DRdigi PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
  $<INSTALL_INTERFACE:include>
)

set_target_properties(DRdigi PROPERTIES PUBLIC_HEADER ${headers})

target_link_libraries(DRdigi
  podio::podioRootIO
  edm4dr
  sipm::sipm
)

install(TARGETS DRdigi
  RUNTIME DESTINATION "${CMAKE_INSTALL_BINDIR}" COMPONENT bin
  PUBLIC_HEADER DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}" COMPONENT dev
)
