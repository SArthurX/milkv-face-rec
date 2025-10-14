#ifndef CONFIG_H
#define CONFIG_H

#include <string>
#include <iostream>

class Config {
public:
    // 模型路徑相關
    std::string models_base_path;
    std::string mtcnn_det1_param, mtcnn_det1_bin;
    std::string mtcnn_det2_param, mtcnn_det2_bin;
    std::string mtcnn_det3_param, mtcnn_det3_bin;
    std::string mtcnn_det4_param, mtcnn_det4_bin;
    std::string arcface_param, arcface_bin;
    
    // 路徑相關
    std::string images_path;
    std::string test_images_path;
    std::string results_path;     // 辨識後的照片輸出目錄
    std::string features_path;    // 人臉特徵圖片輸出目錄
    std::string database_path;    // 人臉資料庫檔案目錄
    
    // 閾值設定
    double face_similarity_threshold;
    double detection_confidence_threshold;
    
    // 設定選項
    bool create_directories;
    bool save_detected_faces;
    bool save_detection_boxes;
    
    // 單例模式
    static Config& getInstance();
    
    // 載入配置文件
    bool loadConfig(const std::string& config_file = "config.json");
    
    // 獲取完整路徑的輔助函數
    std::string getModelPath(const std::string& relative_path) const;
    std::string getImagePath(const std::string& filename) const;
    std::string getResultPath(const std::string& filename) const;
    std::string getFeaturePath(const std::string& filename) const;
    std::string getDatabasePath(const std::string& filename) const;
    
    // 創建目錄
    bool createDirectories() const;

private:
    Config() = default;
    ~Config() = default;
    Config(const Config&) = delete;
    Config& operator=(const Config&) = delete;
    
    // 路徑處理輔助函數
    std::string joinPath(const std::string& base, const std::string& relative) const;
    bool directoryExists(const std::string& path) const;
    bool createDirectory(const std::string& path) const;
};

#endif // CONFIG_H
