cmake_minimum_required(VERSION 3.11)

set(PROJECT_NAME "pbrt-cpp")
project(${PROJECT_NAME})
set(SOURCE_FILES main.cpp)


#FIND_PACKAGE ( ZLIB )
#IF(NOT ZLIB_FOUND)
#  SET(ZLIB_LIBRARY zlibstatic)
#endif()



INCLUDE_DIRECTORIES (
  external/pbrt-v3/src
  ${CMAKE_BINARY_DIR}/external/pbrt-v3/src
  
  external/pbrt-v3/src/ext/glog
  ${CMAKE_BINARY_DIR}/external/pbrt-v3/src/ext/glog

  external/pbrt-v3/src/ext/openexr/IlmBase/Imath
  external/pbrt-v3/src/ext/openexr/IlmBase/Half
  external/pbrt-v3/src/ext/openexr/IlmBase/Iex
  external/pbrt-v3/src/ext/openexr/OpenEXR/IlmImf
  ${CMAKE_BINARY_DIR}/external/pbrt-v3/src/ext/openexr/IlmBase/config
  ${CMAKE_BINARY_DIR}/external/pbrt-v3/src/ext/openexr/OpenEXR/config
)

set(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/external/pbrt-v3/cmake )
  
add_definitions( -D PBRT_CONSTEXPR=constexpr)
add_definitions( -D PBRT_FLOAT_AS_DOUBLE )
ADD_SUBDIRECTORY(external/pbrt-v3)


add_executable(${PROJECT_NAME} ${SOURCE_FILES})
target_compile_features(${PROJECT_NAME} PRIVATE cxx_std_17)


target_link_libraries(${PROJECT_NAME}
pbrt
Half
IlmImf
Iex
IlmThread
Ptex_static
IexMath
${ZLIB_LIBRARY}
glog
)
