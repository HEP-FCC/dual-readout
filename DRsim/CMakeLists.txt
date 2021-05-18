project(DRsim)

include_directories(
  ${CMAKE_CURRENT_SOURCE_DIR}/include
)

file(GLOB sources ${PROJECT_SOURCE_DIR}/src/*.cc)
file(GLOB headers ${PROJECT_SOURCE_DIR}/include/*.hh)

add_executable(DRsim DRsim.cc ${sources} ${headers})

target_include_directories(
  DRsim PUBLIC
  ${CMAKE_CURRENT_SOURCE_DIR}/include
  ${HEPMC3_INCLUDE_DIR}
  ${HEPMC3_INTERFACES_DIR}
)

target_link_libraries(
  DRsim
  ${HEPMC3_LIBRARIES}
  ${HEPMC3_ROOTIO_LIB}
  DRcomponents
  ddDRcalo
  ddDRsensitive
  DRSimG4Components
  DRSimG4Full
  DRSimG4Fast
)

file(GLOB DRsim_MACROS ${PROJECT_SOURCE_DIR}/*.mac compacts.txt)
file(COPY ${DRsim_MACROS} DESTINATION ${PROJECT_BINARY_DIR})

install(TARGETS DRsim
  RUNTIME DESTINATION "${CMAKE_INSTALL_BINDIR}" COMPONENT bin
  PUBLIC_HEADER DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}" COMPONENT dev
)

install(FILES ${DRsim_MACROS} DESTINATION .)
