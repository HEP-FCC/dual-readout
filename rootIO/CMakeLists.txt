project(rootIO)

find_package(ROOT REQUIRED)

include_directories(
  ${CMAKE_CURRENT_SOURCE_DIR}/include
  ${FASTJET_DIR}/include
)
file(GLOB sources
  ${PROJECT_SOURCE_DIR}/src/*.cc
)
file(GLOB headers
  ${PROJECT_SOURCE_DIR}/include/*Interface.h
)

include(${ROOT_USE_FILE})
ROOT_GENERATE_DICTIONARY(G__rootIO ${headers} LINKDEF ${CMAKE_CURRENT_SOURCE_DIR}/include/LinkDef.h)
add_library(rootIO SHARED ${sources} G__rootIO.cxx)
target_include_directories(rootIO PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include ${ROOT_INCLUDE_DIRS})
target_link_libraries(
  rootIO
  ${FASTJET_DIR}/lib/libfastjet.so
  ${ROOT_LIBRARIES}
)

install(
  TARGETS rootIO
  LIBRARY DESTINATION lib
)

install(
  FILES ${CMAKE_CURRENT_BINARY_DIR}/librootIO_rdict.pcm ${CMAKE_CURRENT_BINARY_DIR}/librootIO.rootmap DESTINATION lib
)
