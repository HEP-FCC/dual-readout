project(ddDRsensitive)

include_directories(
  ${CMAKE_CURRENT_SOURCE_DIR}/include
)

file(GLOB sources
  ${PROJECT_SOURCE_DIR}/src/*.cpp
)

file(GLOB headers
  ${PROJECT_SOURCE_DIR}/include/*.h
)

add_library(ddDRsensitive SHARED ${sources} ${headers})

target_include_directories(ddDRsensitive PUBLIC
  ${CMAKE_CURRENT_SOURCE_DIR}/include
  ${DD4hep_DIR}/include
)

target_link_libraries(
  ddDRsensitive
  ddDRsegmentation
  edm4dr
  edm4dr::edm4drDict
  DD4hep::DDCore
  DD4hep::DDG4
)

dd4hep_generate_rootmap(ddDRsensitive)

install(TARGETS ddDRsensitive
  LIBRARY DESTINATION "${CMAKE_INSTALL_LIBDIR}" COMPONENT shlib
  PUBLIC_HEADER DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}" COMPONENT dev
)
