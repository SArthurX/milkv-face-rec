cmake_minimum_required(VERSION 3.10)
project(face_rec)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_FLAGS "-O3")

set(OpenCV_DIR "/home/sx/milkv/milkv-PaddleOCR-ncnn/lib/opencv/opencv4_riscv/lib/cmake/opencv4/")
find_package(OpenCV REQUIRED)

set(dlib_DIR "/home/sx/milkv/detemb/lib/dlib/dlib_risc/lib/cmake/dlib")
find_package(dlib REQUIRED)

set(SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/src/face_rec_simple.cpp

)
add_executable(main ${SOURCES})

target_link_libraries(main
    dlib::dlib
    ${OpenCV_LIBS}
)


