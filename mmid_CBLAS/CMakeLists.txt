cmake_minimum_required(VERSION 2.5)
project(mmid_CBLAS)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=c11 -Wall")

# Blas library
find_package( BLAS REQUIRED )

set(SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/bin)


set(SOURCE_FILES_C ${SOURCE_DIR}/gnuplot.c)

add_executable(${PROJECT_NAME} ${SOURCE_DIR}/main.c ${SOURCE_FILES_C})
target_link_libraries(${PROJECT_NAME} m blas)