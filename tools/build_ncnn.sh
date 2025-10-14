project_root=$(pwd)
ncnn_path="lib/ncnn"
build_dir=${project_root}/${ncnn_path}/build
install_path=${build_dir}/ncnn


rm -rf ${build_dir}
mkdir -p ${build_dir}
cd ${build_dir}

cmake .. \
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