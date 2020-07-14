project(ddDRcalo)

cmake_minimum_required(VERSION 3.12 FATAL_ERROR)

IF(NOT TARGET DD4hep::DDCore)
  find_package ( DD4hep REQUIRED )
  include ( ${DD4hep_DIR}/cmake/DD4hep.cmake )
  include ( ${DD4hep_DIR}/cmake/DD4hepBuild.cmake )
  dd4hep_configure_output()
ENDIF()

dd4hep_set_compiler_flags()
#==========================================================================
dd4hep_print("|++> ddDRcalo: ROOT version: ${ROOT_VERSION}")

if(NOT ${ROOT_VERSION} VERSION_GREATER_EQUAL 6.18.00)
  dd4hep_print("|++> Not building ddDRcalo test")
  return()
endif()
dd4hep_print("|++> Building ddDRcalo test")

#--------------------------------------------------------------------------
dd4hep_configure_output()

include_directories(
  ${CMAKE_CURRENT_SOURCE_DIR}/interface
)

dd4hep_add_plugin(ddDRcalo SOURCES src/*.cpp
  USES DD4hep::DDCore DD4hep::DDCond ROOT::Core ROOT::Geom ROOT::GenVector ROOT::MathCore)
install(TARGETS ddDRcalo LIBRARY DESTINATION lib)
install(DIRECTORY compact scripts DESTINATION bin )
dd4hep_configure_scripts( ddDRcalo DEFAULT_SETUP WITH_TESTS)
