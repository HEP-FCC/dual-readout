project(ddDRsensitive)

cmake_minimum_required(VERSION 3.12 FATAL_ERROR)

option(WITH_GEANT4_UIVIS "Build example with Geant4 UI and Vis drivers" OFF)
if(WITH_GEANT4_UIVIS)
  find_package(Geant4 REQUIRED ui_all vis_all)
else()
  find_package(Geant4 REQUIRED)
endif()

include(${Geant4_USE_FILE})

include_directories(
  ${CMAKE_CURRENT_SOURCE_DIR}/include
)

file(GLOB sources
  ${PROJECT_SOURCE_DIR}/src/*.cpp
)

add_library(ddDRsensitive SHARED ${sources})
target_include_directories(ddDRsensitive PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include ${DD4hep_DIR}/include)
target_link_libraries(
  ddDRsensitive
  ddDRsegmentation
  ${Geant4_LIBRARIES}
  rootIO
  DD4hep::DDCore
)

dd4hep_generate_rootmap(ddDRsensitive)

install(TARGETS ddDRsensitive LIBRARY DESTINATION lib)
