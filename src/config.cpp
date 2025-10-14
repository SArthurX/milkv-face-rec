#include "config.h"
#include <fstream>
#include <sstream>
#include <sys/stat.h>
#include <sys/types.h>
#include <regex>

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
    
    std::stringstream buffer;
    buffer << file.rdbuf();
    std::string json_content = buffer.str();
    file.close();
    
    try {
        // 解析模型路徑
        models_base_path = parseJsonNestedString(json_content, "models", "base_path");
        
        // 解析 MTCNN 模型文件
        mtcnn_det1_param = parseJsonNestedString(json_content, "mtcnn", "det1_param");
        mtcnn_det1_bin = parseJsonNestedString(json_content, "mtcnn", "det1_bin");
        mtcnn_det2_param = parseJsonNestedString(json_content, "mtcnn", "det2_param");
        mtcnn_det2_bin = parseJsonNestedString(json_content, "mtcnn", "det2_bin");
        mtcnn_det3_param = parseJsonNestedString(json_content, "mtcnn", "det3_param");
        mtcnn_det3_bin = parseJsonNestedString(json_content, "mtcnn", "det3_bin");
        mtcnn_det4_param = parseJsonNestedString(json_content, "mtcnn", "det4_param");
        mtcnn_det4_bin = parseJsonNestedString(json_content, "mtcnn", "det4_bin");
        
        // 解析 ArcFace 模型文件
        arcface_param = parseJsonNestedString(json_content, "arcface", "param");
        arcface_bin = parseJsonNestedString(json_content, "arcface", "bin");
        
        // 解析路徑設定
        images_path = parseJsonNestedString(json_content, "paths", "images");
        test_images_path = parseJsonNestedString(json_content, "paths", "test_images");
        results_path = parseJsonNestedString(json_content, "paths", "results");
        features_path = parseJsonNestedString(json_content, "paths", "features");
        database_path = parseJsonNestedString(json_content, "paths", "database");
        
        // 解析閾值
        face_similarity_threshold = parseJsonNestedDouble(json_content, "thresholds", "face_similarity");
        detection_confidence_threshold = parseJsonNestedDouble(json_content, "thresholds", "detection_confidence");
        
        // 解析設定選項
        create_directories = parseJsonNestedBool(json_content, "settings", "create_directories");
        save_detected_faces = parseJsonNestedBool(json_content, "settings", "save_detected_faces");
        save_detection_boxes = parseJsonNestedBool(json_content, "settings", "save_detection_boxes");
        
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

std::string Config::parseJsonNestedString(const std::string& json, const std::string& parent_key, const std::string& child_key) const {
    // 找到父級 key
    std::string search_parent = "\"" + parent_key + "\"";
    size_t parent_pos = json.find(search_parent);
    if (parent_pos == std::string::npos) {
        throw std::runtime_error("Parent key not found: " + parent_key);
    }
    
    // 找到父級的值部分 (開始的 {)
    size_t parent_value_start = json.find("{", parent_pos);
    if (parent_value_start == std::string::npos) {
        throw std::runtime_error("Parent object start not found for key: " + parent_key);
    }
    
    // 找到對應的結束 }
    int brace_count = 1;
    size_t pos = parent_value_start + 1;
    size_t parent_value_end = parent_value_start;
    
    while (pos < json.length() && brace_count > 0) {
        if (json[pos] == '{') {
            brace_count++;
        } else if (json[pos] == '}') {
            brace_count--;
            if (brace_count == 0) {
                parent_value_end = pos;
            }
        }
        pos++;
    }
    
    if (parent_value_end == parent_value_start) {
        throw std::runtime_error("Parent object end not found for key: " + parent_key);
    }
    
    // 在父級對象中搜索子 key
    std::string parent_content = json.substr(parent_value_start, parent_value_end - parent_value_start + 1);
    std::string search_child = "\"" + child_key + "\"";
    size_t child_pos = parent_content.find(search_child);
    if (child_pos == std::string::npos) {
        throw std::runtime_error("Child key not found: " + child_key + " in parent: " + parent_key);
    }
    
    child_pos = parent_content.find(":", child_pos);
    if (child_pos == std::string::npos) {
        throw std::runtime_error("Invalid JSON format for child key: " + child_key);
    }
    
    child_pos = parent_content.find("\"", child_pos);
    if (child_pos == std::string::npos) {
        throw std::runtime_error("String value not found for child key: " + child_key);
    }
    
    size_t start = child_pos + 1;
    size_t end = parent_content.find("\"", start);
    if (end == std::string::npos) {
        throw std::runtime_error("String value not properly closed for child key: " + child_key);
    }
    
    return parent_content.substr(start, end - start);
}

double Config::parseJsonNestedDouble(const std::string& json, const std::string& parent_key, const std::string& child_key) const {
    // 找到父級 key
    std::string search_parent = "\"" + parent_key + "\"";
    size_t parent_pos = json.find(search_parent);
    if (parent_pos == std::string::npos) {
        throw std::runtime_error("Parent key not found: " + parent_key);
    }
    
    // 找到父級的值部分 (開始的 {)
    size_t parent_value_start = json.find("{", parent_pos);
    if (parent_value_start == std::string::npos) {
        throw std::runtime_error("Parent object start not found for key: " + parent_key);
    }
    
    // 找到對應的結束 }
    int brace_count = 1;
    size_t pos = parent_value_start + 1;
    size_t parent_value_end = parent_value_start;
    
    while (pos < json.length() && brace_count > 0) {
        if (json[pos] == '{') {
            brace_count++;
        } else if (json[pos] == '}') {
            brace_count--;
            if (brace_count == 0) {
                parent_value_end = pos;
            }
        }
        pos++;
    }
    
    // 在父級對象中搜索子 key
    std::string parent_content = json.substr(parent_value_start, parent_value_end - parent_value_start + 1);
    std::string search_child = "\"" + child_key + "\"";
    size_t child_pos = parent_content.find(search_child);
    if (child_pos == std::string::npos) {
        throw std::runtime_error("Child key not found: " + child_key + " in parent: " + parent_key);
    }
    
    child_pos = parent_content.find(":", child_pos);
    if (child_pos == std::string::npos) {
        throw std::runtime_error("Invalid JSON format for child key: " + child_key);
    }
    
    // 跳過空格
    child_pos++;
    while (child_pos < parent_content.length() && std::isspace(parent_content[child_pos])) child_pos++;
    
    // 找到數字的結束位置
    size_t start = child_pos;
    while (child_pos < parent_content.length() && (std::isdigit(parent_content[child_pos]) || parent_content[child_pos] == '.')) child_pos++;
    
    std::string value_str = parent_content.substr(start, child_pos - start);
    return std::stod(value_str);
}

bool Config::parseJsonNestedBool(const std::string& json, const std::string& parent_key, const std::string& child_key) const {
    // 找到父級 key
    std::string search_parent = "\"" + parent_key + "\"";
    size_t parent_pos = json.find(search_parent);
    if (parent_pos == std::string::npos) {
        throw std::runtime_error("Parent key not found: " + parent_key);
    }
    
    // 找到父級的值部分 (開始的 {)
    size_t parent_value_start = json.find("{", parent_pos);
    if (parent_value_start == std::string::npos) {
        throw std::runtime_error("Parent object start not found for key: " + parent_key);
    }
    
    // 找到對應的結束 }
    int brace_count = 1;
    size_t pos = parent_value_start + 1;
    size_t parent_value_end = parent_value_start;
    
    while (pos < json.length() && brace_count > 0) {
        if (json[pos] == '{') {
            brace_count++;
        } else if (json[pos] == '}') {
            brace_count--;
            if (brace_count == 0) {
                parent_value_end = pos;
            }
        }
        pos++;
    }
    
    // 在父級對象中搜索子 key
    std::string parent_content = json.substr(parent_value_start, parent_value_end - parent_value_start + 1);
    std::string search_child = "\"" + child_key + "\"";
    size_t child_pos = parent_content.find(search_child);
    if (child_pos == std::string::npos) {
        throw std::runtime_error("Child key not found: " + child_key + " in parent: " + parent_key);
    }
    
    child_pos = parent_content.find(":", child_pos);
    if (child_pos == std::string::npos) {
        throw std::runtime_error("Invalid JSON format for child key: " + child_key);
    }
    
    // 找到 true 或 false
    size_t true_pos = parent_content.find("true", child_pos);
    size_t false_pos = parent_content.find("false", child_pos);
    
    if (true_pos != std::string::npos && (false_pos == std::string::npos || true_pos < false_pos)) {
        return true;
    } else if (false_pos != std::string::npos) {
        return false;
    }
    
    throw std::runtime_error("Boolean value not found for child key: " + child_key);
}

std::string Config::parseJsonString(const std::string& json, const std::string& key) const {
    // 簡單的 JSON 字串解析器
    std::string search_key = "\"" + key + "\"";
    size_t pos = json.find(search_key);
    if (pos == std::string::npos) {
        throw std::runtime_error("Key not found: " + key);
    }
    
    pos = json.find(":", pos);
    if (pos == std::string::npos) {
        throw std::runtime_error("Invalid JSON format for key: " + key);
    }
    
    pos = json.find("\"", pos);
    if (pos == std::string::npos) {
        throw std::runtime_error("String value not found for key: " + key);
    }
    
    size_t start = pos + 1;
    size_t end = json.find("\"", start);
    if (end == std::string::npos) {
        throw std::runtime_error("String value not properly closed for key: " + key);
    }
    
    return json.substr(start, end - start);
}

double Config::parseJsonDouble(const std::string& json, const std::string& key) const {
    std::string search_key = "\"" + key + "\"";
    size_t pos = json.find(search_key);
    if (pos == std::string::npos) {
        throw std::runtime_error("Key not found: " + key);
    }
    
    pos = json.find(":", pos);
    if (pos == std::string::npos) {
        throw std::runtime_error("Invalid JSON format for key: " + key);
    }
    
    // 跳過空格
    pos++;
    while (pos < json.length() && std::isspace(json[pos])) pos++;
    
    // 找到數字的結束位置
    size_t start = pos;
    while (pos < json.length() && (std::isdigit(json[pos]) || json[pos] == '.')) pos++;
    
    std::string value_str = json.substr(start, pos - start);
    return std::stod(value_str);
}

bool Config::parseJsonBool(const std::string& json, const std::string& key) const {
    std::string search_key = "\"" + key + "\"";
    size_t pos = json.find(search_key);
    if (pos == std::string::npos) {
        throw std::runtime_error("Key not found: " + key);
    }
    
    pos = json.find(":", pos);
    if (pos == std::string::npos) {
        throw std::runtime_error("Invalid JSON format for key: " + key);
    }
    
    // 找到 true 或 false
    size_t true_pos = json.find("true", pos);
    size_t false_pos = json.find("false", pos);
    
    if (true_pos != std::string::npos && (false_pos == std::string::npos || true_pos < false_pos)) {
        return true;
    } else if (false_pos != std::string::npos) {
        return false;
    }
    
    throw std::runtime_error("Boolean value not found for key: " + key);
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

std::string Config::getFeaturePath(const std::string& filename) const {
    return joinPath(features_path, filename);
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
    success &= createDirectory(features_path);
    success &= createDirectory(database_path);
    
    if (!success) {
        std::cerr << "Warning: Failed to create some directories" << std::endl;
    }
    
    return success;
}
