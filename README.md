# Face Recognition with dlib and OpenCV on RISC-V

This project demonstrates face recognition using dlib and OpenCV libraries, specifically optimized for RISC-V architecture (Milk-V boards).

## Project Structure

```
detemb/
├── CMakeLists.txt          # Main CMake configuration
├── riscv-toolchain.cmake   # RISC-V cross-compilation toolchain
├── src/
│   └── face_rec_simple.cpp # Main face recognition application
├── lib/
│   └── dlib/               # dlib library source and builds
├── tool/
│   └── build_dlib.sh       # Script to build dlib for RISC-V
├── build/                  # Build directory (generated)
├── test*.jpg               # Test images
└── yolov8n-face.pt         # YOLO face detection model
```

## Features

- Face detection using dlib's HOG-based detector
- Face landmark detection (68-point facial landmarks)
- Cross-platform compilation for RISC-V architecture
- Optimized for Milk-V boards

## Prerequisites

### Host System Requirements
- CMake 3.10 or higher
- RISC-V cross-compilation toolchain
- Git

### Target System (Milk-V)
- RISC-V64 processor with Linux
- Sufficient memory for face processing

## Installation and Build

### 1. Clone the Repository
```bash
git clone <repository-url>
cd detemb
```

### 2. Set up RISC-V Toolchain
Ensure your RISC-V toolchain is installed at:
```
/home/sx/milkv/host-tools/gcc/riscv64-linux-musl-x86_64/
```

### 3. Build dlib for RISC-V
```bash
bash tool/build_dlib.sh
```

This will:
- Configure dlib with RISC-V cross-compilation
- Disable GUI support and unnecessary features
- Build and install to `lib/dlib/dlib_risc/`

### 4. Configure and Build Main Project
```bash
mkdir build && cd build
cmake -DCMAKE_TOOLCHAIN_FILE=../riscv-toolchain.cmake ..
make -j4
```

## Usage

### Running on Target Device
1. Copy the compiled `main` executable to your Milk-V board
2. Copy test images (`test1.jpg`, `test2.jpg`, etc.)
3. Run the application:
```bash
./main
```

### Expected Output
The application will:
- Load and process test images
- Detect faces in the images
- Extract 68-point facial landmarks
- Display processing results

## Configuration

### CMake Options
The project uses the following key configurations:
- `CMAKE_CXX_STANDARD`: C++17
- `CMAKE_CXX_FLAGS`: `-O3` (optimized for performance)
- Cross-compilation for RISC-V architecture

### dlib Configuration
- `DLIB_NO_GUI_SUPPORT=ON`: Disable X11/GUI dependencies
- `DLIB_USE_BLAS=OFF`: Use built-in BLAS (avoid external dependencies)
- `DLIB_USE_LAPACK=OFF`: Disable LAPACK (avoid external dependencies)
- Image format support: JPEG and PNG enabled

## Dependencies

### External Libraries
- **OpenCV 4.x**: Computer vision library (RISC-V build)
  - Location: `/home/sx/milkv/milkv-PaddleOCR-ncnn/lib/opencv/opencv4_riscv/`
- **dlib**: Machine learning and computer vision library
  - Built from source with RISC-V toolchain

### System Libraries
- pthread (threading support)
- Standard C++ runtime

## Troubleshooting

### Common Issues

1. **libgomp.so not found**
   - Solution: The project disables OpenMP to avoid this dependency
   - Ensure `CMAKE_DISABLE_FIND_PACKAGE_OpenMP=TRUE` in toolchain file

2. **OpenCV path issues**
   - Verify OpenCV is built for RISC-V architecture
   - Check `OpenCV_DIR` path in CMakeLists.txt

3. **Cross-compilation errors**
   - Ensure RISC-V toolchain paths are correct
   - Verify sysroot contains necessary libraries

### Build Clean
To perform a clean build:
```bash
rm -rf build
mkdir build && cd build
cmake -DCMAKE_TOOLCHAIN_FILE=../riscv-toolchain.cmake ..
make -j4
```

## Performance Notes

- Optimized for RISC-V architecture
- Uses built-in BLAS for mathematical operations
- Face detection performance depends on image size and CPU capabilities
- Consider reducing image resolution for better performance on embedded systems

## Development

### Adding New Features
1. Modify `src/face_rec_simple.cpp`
2. Update CMakeLists.txt if adding new dependencies
3. Rebuild using the standard build process

### Testing
- Use provided test images for validation
- Test on both x86 (for development) and RISC-V (for deployment)

## Architecture Considerations

### RISC-V Specific Optimizations
- Uses musl libc for smaller footprint
- Configured for `lib64xthead/lp64d` ABI
- Optimized compiler flags for RISC-V performance

### Memory Usage
- Face detection: ~10-50MB depending on image size
- Landmark detection: Additional ~5-10MB per face
- Consider image resizing for memory-constrained systems

## File Descriptions

- **`CMakeLists.txt`**: Main build configuration with cross-compilation settings
- **`riscv-toolchain.cmake`**: Toolchain file defining RISC-V compiler paths and settings
- **`src/face_rec_simple.cpp`**: Main application implementing face detection and landmark extraction
- **`tool/build_dlib.sh`**: Automated script to build dlib with proper RISC-V configuration
- **`yolov8n-face.pt`**: Pre-trained YOLO model for face detection (alternative to dlib detector)

## Quick Start

For a quick test run:
```bash
# Build everything
bash tool/build_dlib.sh
mkdir build && cd build
cmake -DCMAKE_TOOLCHAIN_FILE=../riscv-toolchain.cmake ..
make -j4

# The executable 'main' will be ready for deployment to RISC-V target
```

## License

This project follows the license terms in the [LICENSE](LICENSE) file.

## Contributing

Issues and Pull Requests are welcome to improve this project!

