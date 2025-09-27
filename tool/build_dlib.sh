

rm -rf lib/dlib/build
mkdir -p lib/dlib/build

cd lib/dlib/build && cmake .. \
  -DCMAKE_TOOLCHAIN_FILE=../../../riscv-toolchain.cmake \
  -DCMAKE_INSTALL_PREFIX=/home/sx/milkv/detemb/lib/dlib/build/dlib_risc \
  -DDLIB_NO_GUI_SUPPORT=ON \
  -DDLIB_JPEG_SUPPORT=ON \
  -DDLIB_PNG_SUPPORT=ON \
  -DDLIB_GIF_SUPPORT=OFF \
  -DDLIB_WEBP_SUPPORT=OFF \
  -DDLIB_JXL_SUPPORT=OFF \
  -DDLIB_USE_FFMPEG=OFF \
  -DDLIB_USE_BLAS=OFF \
  -DDLIB_USE_LAPACK=OFF \

make -j4
make install