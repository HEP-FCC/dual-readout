cmake_minimum_required(VERSION 3.12 FATAL_ERROR)
project(ddDR)

IF(NOT TARGET DD4hep::DDCore)
  find_package ( DD4hep REQUIRED )
  include ( ${DD4hep_DIR}/cmake/DD4hep.cmake )
  include ( ${DD4hep_DIR}/cmake/DD4hepBuild.cmake )
  dd4hep_configure_output()
ENDIF()

dd4hep_set_compiler_flags()
#==========================================================================
dd4hep_print("|++> ddDRcalo: ROOT version: ${ROOT_VERSION}")
#--------------------------------------------------------------------------
dd4hep_configure_output()

add_subdirectory(DRcalo)
add_subdirectory(DRsensitive)
add_subdirectory(DRsegmentation)
