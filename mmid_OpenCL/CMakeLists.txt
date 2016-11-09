cmake_minimum_required(VERSION 2.5)
project(mmid_OpenCL)

find_package(OpenCL)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=c11")

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/bin)
set(SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src)

add_executable(embed_kernels ${CMAKE_CURRENT_SOURCE_DIR}/src/embed_kernels.c)
add_custom_command(
        OUTPUT ${SOURCE_DIR}/cl_kernels/kernels.c
        COMMAND embed_kernels ${SOURCE_DIR}/cl_kernels/ ${SOURCE_DIR}/cl_kernels/kernels.c)

set(SOURCE_FILES_CPP)
set(SOURCE_FILES_C ${SOURCE_DIR}/cl_kernels/kernels.c ${SOURCE_DIR}/cl_kernels/opencl_commons.c ${SOURCE_DIR}/cl_kernels/commons.c)

add_executable(${PROJECT_NAME} ${SOURCE_DIR}/main.cpp ${SOURCE_FILES_CPP} ${SOURCE_FILES_C})
set_source_files_properties(${SOURCE_FILES_C} PROPERTIES LANGUAGE CXX)
target_link_libraries(${PROJECT_NAME} OpenCL m)