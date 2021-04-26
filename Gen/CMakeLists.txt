project(P8ptcgen)

find_package(Pythia8 REQUIRED)
find_package(HepMC3 REQUIRED)
find_package(FastJet REQUIRED)

include_directories(
  ${CMAKE_CURRENT_SOURCE_DIR}/include
  ${PYTHIA8_INCLUDE_DIR}
  ${HEPMC3_INCLUDE_DIR}
  ${FASTJET_INCLUDE_DIR}
)
file(GLOB sources
  ${PROJECT_SOURCE_DIR}/src/*.cc
)
file(GLOB headers
  ${PROJECT_SOURCE_DIR}/include/*.h
)

add_executable(P8ptcgen P8ptcgen.cc ${sources} ${headers})
target_link_libraries(
  P8ptcgen
  ${PYTHIA8_LIBRARIES}
  ${HEPMC3_LIBRARIES}
  ${FASTJET_LIBRARY}
  rootIO
  ${CMAKE_DL_LIBS}
)

add_executable(P8generic P8generic.cc ${sources} ${headers})
target_link_libraries(
  P8generic
  ${PYTHIA8_LIBRARIES}
  ${HEPMC3_LIBRARIES}
  ${FASTJET_LIBRARY}
  rootIO
  ${CMAKE_DL_LIBS}
)

set(
  P8_SCRIPTS
  ptcgun.cmnd
  generic.cmnd
)

foreach(_script ${P8_SCRIPTS})
  configure_file(
    ${PROJECT_SOURCE_DIR}/${_script}
    ${PROJECT_BINARY_DIR}/${_script}
    COPYONLY
  )
endforeach()

install(TARGETS P8ptcgen P8generic DESTINATION bin)
install(FILES ${PROJECT_BINARY_DIR}/ptcgun.cmnd ${PROJECT_BINARY_DIR}/generic.cmnd DESTINATION .)
