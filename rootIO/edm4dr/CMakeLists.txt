# For now unconditionally generate all the code that is supported by the
# installed podio
find_package(EDM4HEP REQUIRED)

PODIO_GENERATE_DATAMODEL(edm4hep ../edm4dr.yaml headers sources IO_BACKEND_HANDLERS ${PODIO_IO_HANDLERS})

# use customized library linking instead of PODIO_ADD_DATAMODEL_CORE_LIB(edm4dr "${headers}" "${sources}")

# Filter out anything I/O backend related to build the core library
LIST(FILTER headers EXCLUDE REGEX .*SIOBlock.h)
LIST(FILTER sources EXCLUDE REGEX .*SIOBlock.cc)

add_library(edm4dr SHARED ${sources} ${headers})
target_link_libraries(edm4dr
  PUBLIC
  EDM4HEP::edm4hep
  podio::podio
)
target_include_directories(edm4dr
  PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
  $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)
set_target_properties(edm4dr PROPERTIES PUBLIC_HEADER "${headers}")


PODIO_ADD_ROOT_IO_DICT(edm4drDict edm4dr "${headers}" src/selection.xml)

add_library(edm4dr::edm4drDict ALIAS edm4drDict)

list(APPEND EDM4DR_INSTALL_LIBS edm4dr edm4drDict)

PODIO_ADD_SIO_IO_BLOCKS(edm4dr "${headers}" "${sources}")
IF(TARGET edm4drSioBlocks)
  message(STATUS "Building and installing the SioBlocks since podio supports it")
  list(APPEND EDM4DR_INSTALL_LIBS edm4drSioBlocks)
ENDIF()

install(TARGETS ${EDM4DR_INSTALL_LIBS}
  EXPORT EDM4DRTargets
  RUNTIME DESTINATION "${CMAKE_INSTALL_BINDIR}" COMPONENT bin
  LIBRARY DESTINATION "${CMAKE_INSTALL_LIBDIR}" COMPONENT shlib
  PUBLIC_HEADER DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}/edm4hep" COMPONENT dev
)

install(FILES
  "${CMAKE_CURRENT_BINARY_DIR}/edm4drDictDict.rootmap"
  DESTINATION "${CMAKE_INSTALL_LIBDIR}" COMPONENT dev
)

if (${ROOT_VERSION} GREATER 6)
  install(FILES
    "${CMAKE_CURRENT_BINARY_DIR}/libedm4drDict_rdict.pcm"
    DESTINATION "${CMAKE_INSTALL_LIBDIR}" COMPONENT dev
  )
endif()
