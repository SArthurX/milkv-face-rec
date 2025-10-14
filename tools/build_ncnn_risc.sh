project_root=$(pwd)
ncnn_path="lib/ncnn"
build_dir=${project_root}/${ncnn_path}/build
install_path=${build_dir}/ncnn_riscv


# 設置 RISCV_ROOT_PATH 環境變量
export RISCV_ROOT_PATH="${project_root}/tools/toolchain/gcc/riscv64-linux-musl-x86_64"

rm -rf ${build_dir}
mkdir -p ${build_dir}
cd ${build_dir}

cmake .. \
  -DCMAKE_SYSTEM_NAME=Linux \
  -DCMAKE_SYSTEM_PROCESSOR=riscv64 \
  -DCMAKE_C_COMPILER="${RISCV_ROOT_PATH}/bin/riscv64-unknown-linux-musl-gcc" \
  -DCMAKE_CXX_COMPILER="${RISCV_ROOT_PATH}/bin/riscv64-unknown-linux-musl-g++" \
  -DCMAKE_FIND_ROOT_PATH="${RISCV_ROOT_PATH}/sysroot" \
  -DCMAKE_FIND_ROOT_PATH_MODE_PROGRAM=NEVER \
  -DCMAKE_FIND_ROOT_PATH_MODE_LIBRARY=ONLY \
  -DCMAKE_FIND_ROOT_PATH_MODE_INCLUDE=ONLY \
  -DCMAKE_FIND_ROOT_PATH_MODE_PACKAGE=ONLY \
  -DCMAKE_C_FLAGS="-march=rv64gc" \
  -DCMAKE_CXX_FLAGS="-march=rv64gc" \
  -DCMAKE_INSTALL_PREFIX=${install_path} \
  -DCMAKE_BUILD_TYPE=Release \
  -DNCNN_VULKAN=OFF \
  -DNCNN_OPENMP=OFF \
  -DNCNN_RUNTIME_CPU=OFF \
  -DNCNN_AVX2=OFF \
  -DNCNN_AVX=OFF \
  -DNCNN_F16C=OFF \
  -DNCNN_FMA=OFF \
  -DNCNN_XOP=OFF

make -j$(nproc)
make install