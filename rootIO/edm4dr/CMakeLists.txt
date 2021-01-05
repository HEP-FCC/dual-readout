# For now unconditionally generate all the code that is supported by the
# installed podio
find_package(EDM4HEP REQUIRED)

PODIO_GENERATE_DATAMODEL(edm4hep ../edm4dr.yaml headers sources IO_BACKEND_HANDLERS ${PODIO_IO_HANDLERS})

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

add_library(edm4drDict SHARED)
add_library(edm4dr::edm4drDict ALIAS edm4drDict )
target_link_libraries(edm4drDict
  PUBLIC
  edm4dr
  podio::podio
  ROOT::Core
)
target_include_directories(edm4drDict
  PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
  $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
  ${ROOT_INCLUDE_DIRS}
)

PODIO_GENERATE_DICTIONARY(edm4drDict ${headers} SELECTION src/selection.xml
  OPTIONS --library ${CMAKE_SHARED_LIBRARY_PREFIX}edm4drDict${CMAKE_SHARED_LIBRARY_SUFFIX} )
set_target_properties(edm4drDict-dictgen PROPERTIES EXCLUDE_FROM_ALL TRUE)
add_dependencies(edm4drDict edm4dr)

list(APPEND edm4dr_INSTALL_LIBS edm4dr edm4drDict)

install(TARGETS ${edm4dr_INSTALL_LIBS}
  EXPORT edm4drTargets
  RUNTIME DESTINATION "${CMAKE_INSTALL_BINDIR}" COMPONENT bin
  LIBRARY DESTINATION "${CMAKE_INSTALL_LIBDIR}" COMPONENT shlib
  PUBLIC_HEADER DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}/edm4hep"
  COMPONENT dev)

install(FILES
  "${CMAKE_CURRENT_BINARY_DIR}/edm4drDictDict.rootmap"
  DESTINATION lib COMPONENT dev)

if (${ROOT_VERSION} GREATER 6)
  install(FILES
      "${CMAKE_CURRENT_BINARY_DIR}/libedm4drDict_rdict.pcm"
      DESTINATION lib COMPONENT dev)
endif()
