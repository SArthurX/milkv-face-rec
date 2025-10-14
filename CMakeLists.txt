cmake_minimum_required(VERSION 3.10)
project(face_rec)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_FLAGS "-O3 -fopenmp -pthread")

# 設置 OpenCV 路徑
set(OpenCV_DIR "${CMAKE_SOURCE_DIR}/lib/opencv/build/opencv4/lib/cmake/opencv4" CACHE PATH "OpenCV config path")
find_package(OpenCV REQUIRED COMPONENTS core imgproc imgcodecs)

# 設置 ncnn 路徑
set(ncnn_DIR "${CMAKE_SOURCE_DIR}/lib/ncnn/build/ncnn/lib/cmake/ncnn" CACHE PATH "ncnn cmake config path for RISC-V")
find_package(ncnn REQUIRED)

# 共用的原始檔案 (基於 Makefile 中的 OBJ)
set(COMMON_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/src/base.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/mtcnn.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/arcface.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/config.cpp
)

# 人臉資料庫示例程式源文件
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
)

# 複製配置文件到構建目錄
configure_file(${CMAKE_SOURCE_DIR}/config.json ${CMAKE_BINARY_DIR}/config.json COPYONLY)




