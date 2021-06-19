project(ddDRcalo)

include_directories(
  ${CMAKE_CURRENT_SOURCE_DIR}/include
)

dd4hep_add_plugin(ddDRcalo SOURCES src/*.cpp USES
  DD4hep::DDCore
  DD4hep::DDCond
  ROOT::Core
  ROOT::Geom
  ROOT::GenVector
  ROOT::MathCore
  DRsegmentation
)

install(TARGETS ddDRcalo
  LIBRARY DESTINATION "${CMAKE_INSTALL_LIBDIR}" COMPONENT shlib
  PUBLIC_HEADER DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}" COMPONENT dev
)

install(DIRECTORY
  compact DESTINATION "${CMAKE_INSTALL_BINDIR}" COMPONENT bin
)

dd4hep_configure_scripts( ddDRcalo DEFAULT_SETUP WITH_TESTS )
