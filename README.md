# Face Recognition System - RISC-V Edition

A high-performance face detection and recognition system based on MTCNN + ArcFace, optimized for RISC-V architecture (Milk-V development boards).

## 🚀 Features

- **Face Detection**: Precise face detection using MTCNN multi-stage cascaded networks
- **Face Recognition**: High-accuracy face feature extraction and matching based on ArcFace model
- **Face Database**: Support for person registration, recognition, and management
- **JSON Configuration**: Modular configuration management using nlohmann/json library
- **Cross-platform Compilation**: Optimized for RISC-V architecture with Milk-V board support

## 📁 Project Structure

```
detemb/
├── CMakeLists.txt              # Main CMake configuration
├── config.json                 # Default configuration file
├── riscv-toolchain.cmake       # RISC-V cross-compilation toolchain
├── src/                        # Source code directory
│   ├── main.cpp               # Main program entry
│   ├── config.h/.cpp          # Configuration management system
│   ├── mtcnn.h/.cpp           # MTCNN face detection
│   ├── arcface.h/.cpp         # ArcFace face recognition
│   ├── face_database.h/.cpp   # Face database management
│   ├── base.h/.cpp            # Basic utility functions
│   └── 3rdparty/              # Third-party libraries
│       └── json/              # nlohmann/json library
├── lib/                        # Dependency libraries
│   ├── ncnn/                  # NCNN inference engine
│   └── opencv/                # OpenCV computer vision library
├── models/                     # AI model files
│   ├── det1.param/.bin        # MTCNN P-Net
│   ├── det2.param/.bin        # MTCNN R-Net
│   ├── det3.param/.bin        # MTCNN O-Net
│   ├── det4.param/.bin        # MTCNN L-Net
│   └── mobilefacenet.param/.bin # ArcFace model
├── images/                     # Input images directory
├── test/                      # Test images directory
├── results/                   # Recognition results output
├── features/                  # Face feature files
├── database/                  # Face database files
└── tools/                     # Build tools and scripts
```

## ⚙️ System Requirements

### Host System (Build Environment)
- Ubuntu 20.04+ or CentOS 8+
- CMake 3.10 or higher
- RISC-V cross-compilation toolchain
- Git

### Target System (Milk-V Board)
- RISC-V64 processor + Linux
- Memory: Recommended 2GB or more
- Storage: Recommended 1GB available space

## 🔧 Installation & Build

### 1. Clone the Project
```bash
git clone https://github.com/SArthurX/milkv-face-rec.git
cd milkv-face-rec
```

### 2. Build Dependencies
```bash
# Build NCNN (for RISC-V)
bash tools/build_ncnn_risc.sh

# Build OpenCV (for RISC-V)
bash tools/build_opencv_risc.sh
```

### 3. Compile Main Program
```bash
# Clean and build
rm -rf build && mkdir build && cd build
cmake -DCMAKE_TOOLCHAIN_FILE=../riscv-toolchain.cmake ..
make -j$(nproc)
```

### 4. Deploy to Target Device
```bash
# Copy compiled executable and config files to Milk-V
scp build/main config.json root@milkv-ip:/root/face-rec/
scp -r models/ images/ root@milkv-ip:/root/face-rec/
```

## 🎯 Usage

### Basic Command Format
```bash
./main [-c config.json] <command> [args...]
```

### Person Registration
```bash
# Using default configuration
./main register "John" "john.jpg"

# Using custom configuration file
./main -c production.json register "Alice" "alice.jpg"
```

### Face Recognition
```bash
# Recognize single image
./main recognize "unknown_person.jpg"

# Using custom configuration
./main -c custom.json recognize "test_image.jpg"
```

### Database Management
```bash
# List all registered persons
./main list

# Remove specific person
./main remove "John"
```

## 📋 Configuration File

### config.json Structure
```json
{
    "models": {
        "base_path": "./models",
        "mtcnn": {
            "det1_param": "det1.param",
            "det1_bin": "det1.bin",
            "det2_param": "det2.param",
            "det2_bin": "det2.bin",
            "det3_param": "det3.param",
            "det3_bin": "det3.bin",
            "det4_param": "det4.param",
            "det4_bin": "det4.bin"
        },
        "arcface": {
            "param": "mobilefacenet.param",
            "bin": "mobilefacenet.bin"
        }
    },
    "paths": {
        "images": "./images",
        "test_images": "./test",
        "results": "./results",
        "features": "./features",
        "database": "./database"
    },
    "thresholds": {
        "face_similarity": 0.6,
        "detection_confidence": 0.8
    },
    "settings": {
        "create_directories": true,
        "save_detected_faces": true,
        "save_detection_boxes": true
    }
}
```

### Configuration Options
- **models.base_path**: Base path for model files
- **thresholds.face_similarity**: Face similarity threshold (0.0-1.0)
- **thresholds.detection_confidence**: Face detection confidence threshold
- **settings.create_directories**: Auto-create directories
- **settings.save_detected_faces**: Save detected face images
- **settings.save_detection_boxes**: Draw detection boxes on result images

## 🔍 Troubleshooting

### Common Issues

1. **Model files not found**
   ```bash
   # Check model file paths
   ls -la models/
   # Verify path settings in configuration
   ```

2. **Out of memory**
   ```bash
   # Adjust image resolution
   # Modify related parameters in configuration
   ```

3. **Cross-compilation errors**
   ```bash
   # Verify toolchain path
   which riscv64-linux-gnu-gcc
   # Clean and rebuild
   rm -rf build && mkdir build
   ```

### Performance Tuning

- **Image Preprocessing**: Recommend resizing input images to 640x480 or smaller
- **Memory Management**: Consider batch processing for large-scale operations
- **Model Optimization**: Consider using quantized models to reduce memory usage


### Development Environment Setup
1. Fork this project
2. Create feature branch: `git checkout -b feature/amazing-feature`
3. Commit changes: `git commit -m 'Add amazing feature'`
4. Push branch: `git push origin feature/amazing-feature`
5. Open Pull Request


## 📜 License

This project is licensed under the terms in the [LICENSE](LICENSE) file.

## Acknowledgments
* [liguiyuan/mobilefacenet-ncnn](https://github.com/liguiyuan/mobilefacenet-ncnn)
* [nlohmann/json](https://github.com/nlohmann/json)

## Contributing

Issues and Pull Requests are welcome to improve this project!

---

**Note**: This project is for learning and research purposes only. For commercial use, please ensure compliance with relevant regulations.

