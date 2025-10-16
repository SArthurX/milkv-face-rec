cmake_minimum_required(VERSION 3.10)
project(face_rec)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_FLAGS "-O3 -fopenmp -pthread")

set(OpenCV_DIR "${CMAKE_SOURCE_DIR}/lib/opencv/build/opencv4_riscv/lib/cmake/opencv4" CACHE PATH "OpenCV config path")
find_package(OpenCV REQUIRED COMPONENTS core imgproc imgcodecs)

set(ncnn_DIR "${CMAKE_SOURCE_DIR}/lib/ncnn/build/ncnn_riscv/lib/cmake/ncnn" CACHE PATH "ncnn cmake config path for RISC-V")
find_package(ncnn REQUIRED)

set(COMMON_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/src/base.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/mtcnn.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/arcface.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/config.cpp
)

set(MAIN_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/src/main.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/face_database.cpp
    ${COMMON_SOURCES}
)
add_executable(main ${MAIN_SOURCES})

target_link_libraries(main
    ${OpenCV_LIBS}
    ncnn
    m
)

target_include_directories(main PRIVATE
    ${OpenCV_INCLUDE_DIRS}
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${CMAKE_CURRENT_SOURCE_DIR}/src/3rdparty
)