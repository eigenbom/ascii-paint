cmake_minimum_required(VERSION 2.6)
PROJECT(ASCIIPAINT)

set(BASE_DIR ${ASCIIPAINT_SOURCE_DIR})
set(CMAKE_INSTALL_PREFIX ${BASE_DIR}/bin)



SET(CMAKE_BUILD_TYPE DEBUG)

IF(CMAKE_COMPILER_IS_GNUCXX)
	SET(CMAKE_EXE_LINKER_FLAGS "-Wl,--allow-multiple-definition")
ENDIF(CMAKE_COMPILER_IS_GNUCXX)

include_directories(include)
include_directories(include/gd)

link_directories(${BASE_DIR}/lib)

add_subdirectory(${BASE_DIR}/src)

