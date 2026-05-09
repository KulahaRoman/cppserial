cmake_minimum_required(VERSION 3.15)

project(lib-cppserial)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CPPSERIAL_INSTALL_BIN ${CMAKE_CURRENT_SOURCE_DIR}/bin)
set(CPPSERIAL_INSTALL_LIB ${CMAKE_CURRENT_SOURCE_DIR}/lib)
set(CPPSERIAL_INSTALL_INCLUDE ${CMAKE_CURRENT_SOURCE_DIR}/include)

if(NOT TARGET gtest)
	add_subdirectory(libs/gtest)
endif()

if(NOT TARGET cppserial)
	add_subdirectory(src)
	add_subdirectory(test)
endif()