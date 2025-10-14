#!/bin/bash
project_root=$(pwd)
opencv_path="lib/opencv"
build_dir=${project_root}/${opencv_path}/build
install_path=${build_dir}/opencv4_riscv

rm -rf ${build_dir}
mkdir -p ${build_dir}
cd ${build_dir}

RISCV_TOOLCHAIN_PATH="${project_root}/tools/toolchain/gcc/riscv64-linux-musl-x86_64"
RISCV_SYSROOT="${project_root}/tools/toolchain/gcc/riscv64-linux-musl-x86_64/sysroot"

cmake .. \
    -DCMAKE_INSTALL_PREFIX=${install_path} \
    -DCMAKE_BUILD_TYPE=Release \
    -DCMAKE_SYSTEM_NAME=Linux \
    -DCMAKE_SYSTEM_PROCESSOR=riscv64 \
    -DCMAKE_C_COMPILER=${RISCV_TOOLCHAIN_PATH}/bin/riscv64-unknown-linux-musl-gcc \
    -DCMAKE_CXX_COMPILER=${RISCV_TOOLCHAIN_PATH}/bin/riscv64-unknown-linux-musl-g++ \
    -DCMAKE_FIND_ROOT_PATH=${RISCV_SYSROOT} \
    -DCMAKE_FIND_ROOT_PATH_MODE_PROGRAM=NEVER \
    -DCMAKE_FIND_ROOT_PATH_MODE_LIBRARY=ONLY \
    -DCMAKE_FIND_ROOT_PATH_MODE_INCLUDE=ONLY \
    -DCMAKE_SYSROOT=${RISCV_SYSROOT} \
    -DBUILD_SHARED_LIBS=OFF \
    -DCMAKE_INSTALL_LIBDIR=lib \
    \
    -DWITH_JPEG=ON \
    -DBUILD_JPEG=ON \
    -DWITH_PNG=ON \
    -DBUILD_PNG=ON \
    -DWITH_TIFF=ON \
    -DBUILD_TIFF=ON \
    -DWITH_ZLIB=ON \
    -DBUILD_ZLIB=ON \
    \
    -DBUILD_opencv_core=ON \
    -DBUILD_opencv_imgproc=ON \
    -DBUILD_opencv_imgcodecs=ON \
    -DBUILD_LIST="core,imgproc,imgcodecs" \
    \
    -DWITH_IPP=OFF \
    -DBUILD_IPP_IW=OFF \
    -DWITH_LAPACK=OFF \
    -DWITH_EIGEN=OFF \
    -DWITH_OPENEXR=OFF \
    -DBUILD_OPENEXR=OFF \
    -DWITH_GTK=OFF \
    -DWITH_QT=OFF \
    -DWITH_OPENGL=OFF \
    -DWITH_OPENCL=OFF \
    -DWITH_CUDA=OFF \
    -DWITH_1394=OFF \
    -DWITH_GSTREAMER=OFF \
    -DWITH_FFMPEG=OFF \
    -DWITH_V4L=OFF \
    -DWITH_VTK=OFF \
    -DWITH_PROTOBUF=OFF \
    -DWITH_WEBP=OFF \
    -DWITH_JASPER=OFF \
    -DWITH_OPENJPEG=OFF \
    -DBUILD_OPENJPEG=OFF \
    -DWITH_AVIF=OFF \
    -DWITH_QUIRC=OFF \
    -DWITH_ADE=OFF \
    -DWITH_OPENMP=ON \
    \
    -DBUILD_opencv_apps=OFF \
    -DBUILD_opencv_java=OFF \
    -DBUILD_opencv_python=OFF \
    -DBUILD_opencv_python2=OFF \
    -DBUILD_opencv_python3=OFF \
    -DBUILD_TESTS=OFF \
    -DBUILD_PERF_TESTS=OFF \
    -DBUILD_EXAMPLES=OFF \
    -DBUILD_DOCS=OFF \
    \
    -DBUILD_opencv_calib3d=OFF \
    -DBUILD_opencv_features2d=OFF \
    -DBUILD_opencv_flann=OFF \
    -DBUILD_opencv_highgui=OFF \
    -DBUILD_opencv_ml=OFF \
    -DBUILD_opencv_objdetect=OFF \
    -DBUILD_opencv_photo=OFF \
    -DBUILD_opencv_shape=OFF \
    -DBUILD_opencv_stitching=OFF \
    -DBUILD_opencv_superres=OFF \
    -DBUILD_opencv_video=OFF \
    -DBUILD_opencv_videoio=OFF \
    -DBUILD_opencv_videostab=OFF

make -j$(nproc)
make install

echo "OpenCV for RISC-V Path: ${install_path}"