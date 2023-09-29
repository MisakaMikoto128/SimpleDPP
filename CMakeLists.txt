cmake_minimum_required(VERSION 3.5.0)
project(SimpleDPP VERSION 1.1.0 LANGUAGES C)

include(CTest)
enable_testing()


set(SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src)
set(INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/include)
file(GLOB SOURCES ${SRC_DIR}/*.c)
include_directories(${INCLUDE_DIR})
add_executable(SimpleDPP ${SOURCES})

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
