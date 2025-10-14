#include "config.h"
#include <fstream>
#include <sys/stat.h>
#include <sys/types.h>
#include <json/json.hpp>

using json = nlohmann::json;

Config& Config::getInstance() {
    static Config instance;
    return instance;
}

bool Config::loadConfig(const std::string& config_file) {
    std::ifstream file(config_file);
    if (!file.is_open()) {
        std::cerr << "Error: Cannot open config file: " << config_file << std::endl;
        return false;
    }
    
    try {
        json j;
        file >> j;
        
        // 解析模型路徑 - 一行搞定！
        models_base_path = j["models"]["base_path"];
        
        // 解析 MTCNN 模型文件
        mtcnn_det1_param = j["models"]["mtcnn"]["det1_param"];
        mtcnn_det1_bin = j["models"]["mtcnn"]["det1_bin"];
        mtcnn_det2_param = j["models"]["mtcnn"]["det2_param"];
        mtcnn_det2_bin = j["models"]["mtcnn"]["det2_bin"];
        mtcnn_det3_param = j["models"]["mtcnn"]["det3_param"];
        mtcnn_det3_bin = j["models"]["mtcnn"]["det3_bin"];
        mtcnn_det4_param = j["models"]["mtcnn"]["det4_param"];
        mtcnn_det4_bin = j["models"]["mtcnn"]["det4_bin"];
        
        // 解析 ArcFace 模型文件
        arcface_param = j["models"]["arcface"]["param"];
        arcface_bin = j["models"]["arcface"]["bin"];
        
        // 解析路徑設定
        images_path = j["paths"]["images"];
        test_images_path = j["paths"]["test_images"];
        results_path = j["paths"]["results"];
        database_path = j["paths"]["database"];
        
        // 解析閾值
        face_similarity_threshold = j["thresholds"]["face_similarity"];
        detection_confidence_threshold = j["thresholds"]["detection_confidence"];
        
        // 解析設定選項
        create_directories = j["settings"]["create_directories"];
        save_detected_faces = j["settings"]["save_detected_faces"];
        save_detection_boxes = j["settings"]["save_detection_boxes"];
        
        // 如果設定為自動創建目錄，則創建所需目錄
        if (create_directories) {
            createDirectories();
        }
        
        return true;
    } catch (const std::exception& e) {
        std::cerr << "Error parsing config file: " << e.what() << std::endl;
        return false;
    }
}

std::string Config::joinPath(const std::string& base, const std::string& relative) const {
    if (base.empty()) return relative;
    if (relative.empty()) return base;
    
    if (base.back() == '/' || relative.front() == '/') {
        return base + relative;
    } else {
        return base + "/" + relative;
    }
}

std::string Config::getModelPath(const std::string& relative_path) const {
    return joinPath(models_base_path, relative_path);
}

std::string Config::getImagePath(const std::string& filename) const {
    return joinPath(images_path, filename);
}

std::string Config::getTestImagePath(const std::string& filename) const {
    return joinPath(test_images_path, filename);
}

std::string Config::getResultPath(const std::string& filename) const {
    return joinPath(results_path, filename);
}


std::string Config::getDatabasePath(const std::string& filename) const {
    return joinPath(database_path, filename);
}

bool Config::directoryExists(const std::string& path) const {
    struct stat info;
    return stat(path.c_str(), &info) == 0 && S_ISDIR(info.st_mode);
}

bool Config::createDirectory(const std::string& path) const {
    if (directoryExists(path)) {
        return true;
    }
    
    // 遞歸創建父目錄
    size_t pos = path.find_last_of('/');
    if (pos != std::string::npos) {
        std::string parent = path.substr(0, pos);
        if (!createDirectory(parent)) {
            return false;
        }
    }
    
    return mkdir(path.c_str(), 0755) == 0;
}

bool Config::createDirectories() const {
    bool success = true;
    
    success &= createDirectory(models_base_path);
    success &= createDirectory(images_path);
    success &= createDirectory(test_images_path);
    success &= createDirectory(results_path);
    success &= createDirectory(database_path);
    
    if (!success) {
        std::cerr << "Warning: Failed to create some directories" << std::endl;
    }
    
    return success;
}
