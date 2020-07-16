project(ddDRsegmentation)

cmake_minimum_required(VERSION 3.12 FATAL_ERROR)

include_directories(
  ${CMAKE_CURRENT_SOURCE_DIR}/include
)

file(GLOB sources
  ${PROJECT_SOURCE_DIR}/src/*.cpp
)

add_library(ddDRsegmentation SHARED ${sources})
target_include_directories(ddDRsegmentation PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include ${DD4hep_DIR}/include)
target_link_libraries(
  ddDRsegmentation
  DD4hep::DDCore
)

dd4hep_generate_rootmap(ddDRsegmentation)

install(TARGETS ddDRsegmentation LIBRARY DESTINATION lib)
