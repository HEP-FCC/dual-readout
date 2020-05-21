project(P8ptcgen)

include_directories(
  ${CMAKE_CURRENT_SOURCE_DIR}/include
  ${PYTHIA_DIR}/include
  ${HEPMC_DIR}/include
  ${FASTJET_DIR}/include
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
  ${PYTHIA_DIR}/lib/libpythia8.a
  ${HEPMC_DIR}/lib64/libHepMC3.so
  ${HEPMC_DIR}/lib64/libHepMC3rootIO.so
  ${FASTJET_DIR}/lib/libfastjet.so
  rootIO
  ${CMAKE_DL_LIBS}
)

add_executable(P8generic P8generic.cc ${sources} ${headers})
target_link_libraries(
  P8generic
  ${PYTHIA_DIR}/lib/libpythia8.a
  ${HEPMC_DIR}/lib64/libHepMC3.so
  ${HEPMC_DIR}/lib64/libHepMC3rootIO.so
  ${FASTJET_DIR}/lib/libfastjet.so
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
