#include <vector>
#include <iostream>
#include <opencv2/opencv.hpp>
#include "arcface.h"
#include "mtcnn.h"
#include "face_database.h"
#include "config.h"

cv::Mat ncnn2cv(ncnn::Mat img)
{
    unsigned char pix[img.h * img.w * 3];
    img.to_pixels(pix, ncnn::Mat::PIXEL_BGR);
    cv::Mat cv_img(img.h, img.w, CV_8UC3);
    for (int i = 0; i < cv_img.rows; i++)
    {
        for (int j = 0; j < cv_img.cols; j++)
        {
            cv_img.at<cv::Vec3b>(i,j)[0] = pix[3 * (i * cv_img.cols + j)];
            cv_img.at<cv::Vec3b>(i,j)[1] = pix[3 * (i * cv_img.cols + j) + 1];
            cv_img.at<cv::Vec3b>(i,j)[2] = pix[3 * (i * cv_img.cols + j) + 2];
        }
    }
    return cv_img;
}

void printUsage() {
    std::cout << "Face Recognition Database Demo" << std::endl;
    std::cout << "Usage:" << std::endl;
    std::cout << "  ./main [-c config.json] <command> [args...]" << std::endl;
    std::cout << "Commands:" << std::endl;
    std::cout << "  register <name> <image_path>  - Register a new person" << std::endl;
    std::cout << "  recognize <image_path>        - Recognize person in image" << std::endl;
    std::cout << "  list                          - List all registered persons" << std::endl;
    std::cout << "  remove <name>                 - Remove a person from database" << std::endl;
    std::cout << "Options:" << std::endl;
    std::cout << "  -c config.json               - Specify config file (default: config.json)" << std::endl;
    std::cout << "Example:" << std::endl;
    std::cout << "  ./main register \"John\" \"john.jpg\"" << std::endl;
    std::cout << "  ./main -c production.json recognize \"unknown.jpg\"" << std::endl;
}

int main(int argc, char* argv[])
{
    if (argc < 2) {
        printUsage();
        return -1;
    }
    
    // 解析命令列參數
    std::string config_file = "config.json";  // 預設配置檔案
    int arg_start = 1;  // 命令開始的參數索引
    
    // 檢查是否指定了配置檔案
    if (argc >= 3 && std::string(argv[1]) == "-c") {
        if (argc < 4) {
            printUsage();
            return -1;
        }
        config_file = argv[2];
        arg_start = 3;  // 命令從第3個參數開始
    }

    std::string command = argv[arg_start];

    // 載入配置文件
    Config& config = Config::getInstance();
    if (!config.loadConfig(config_file)) {
        std::cerr << "Failed to load config file: " << config_file << std::endl;
        return -1;
    }
    
    std::cout << "Using config file: " << config_file << std::endl;
    
    // 初始化模型
    MtcnnDetector detector("");
    Arcface arc("");
    FaceDatabase db(config.getDatabasePath("face_database.txt"));
    
    if (command == "register" && argc == arg_start + 3) {
        std::string name = argv[arg_start + 1];
        std::string image_path = argv[arg_start + 2];

        // 讀取圖片
        cv::Mat img = cv::imread(image_path);
        if (img.empty()) {
            std::cerr << "Error: Cannot read image " << image_path << std::endl;
            return -1;
        }
        
        // 轉換為 ncnn 格式
        ncnn::Mat ncnn_img = ncnn::Mat::from_pixels(img.data, ncnn::Mat::PIXEL_BGR, img.cols, img.rows);
        
        // 檢測人臉
        std::vector<FaceInfo> results = detector.Detect(ncnn_img);
        if (results.empty()) {
            std::cerr << "Error: No face detected in image " << image_path << std::endl;
            return -1;
        }

        std::cout << "Detected " << results.size() << " face(s), using the first one." << std::endl;

        // 預處理人臉
        ncnn::Mat face = preprocess(ncnn_img, results[0]);
        
        // 提取特徵
        std::vector<float> feature = arc.getFeature(face);
        
        // 添加到數據庫
        if (db.addPerson(name, image_path, feature, results[0].score)) {
            std::cout << "Successfully registered " << name << " with confidence " << results[0].score << std::endl;

            // 保存檢測到的人臉到 features 目錄
            if (config.save_detected_faces) {
                cv::Mat face_img = ncnn2cv(face);
                std::string face_filename = name + "_face.jpg";
                std::string full_face_path = config.getFeaturePath(face_filename);
                cv::imwrite(full_face_path, face_img);
                std::cout << "Face image saved as: " << full_face_path << std::endl;
            }
        } else {
            std::cerr << "Error: Failed to register " << name << std::endl;
            return -1;
        }
        
    } else if (command == "recognize" && argc == arg_start + 2) {
        std::string image_path = argv[arg_start + 1];
        
        // 讀取圖片
        cv::Mat img = cv::imread(image_path);
        if (img.empty()) {
            std::cerr << "Error: Cannot read image " << image_path << std::endl;
            return -1;
        }
        
        // 轉換為 ncnn 格式
        ncnn::Mat ncnn_img = ncnn::Mat::from_pixels(img.data, ncnn::Mat::PIXEL_BGR, img.cols, img.rows);
        
        // 檢測人臉
        std::vector<FaceInfo> results = detector.Detect(ncnn_img);
        if (results.empty()) {
            std::cerr << "Error: No face detected in image " << image_path << std::endl;
            return -1;
        }
        
        std::cout << "Detected " << results.size() << " face(s):" << std::endl;
        
        // 在圖片上標註結果
        cv::Mat result_img = img.clone();
        
        for (size_t i = 0; i < results.size(); i++) {
            // 預處理人臉
            ncnn::Mat face = preprocess(ncnn_img, results[i]);
            
            // 提取特徵
            std::vector<float> feature = arc.getFeature(face);
            
            // 在數據庫中搜索
            auto match = db.searchPerson(feature, 0.6);

            std::cout << "Face " << (i+1) << ": " << match.first << " (similarity: " << match.second << ")" << std::endl;
            
            // 在圖片上畫框和標籤
            FaceInfo& face_info = results[i];
            cv::rectangle(result_img, 
                         cv::Point(face_info.x[0], face_info.y[0]), 
                         cv::Point(face_info.x[1], face_info.y[1]), 
                         cv::Scalar(0, 255, 0), 2);
            
            // 添加文字標籤
            std::string label = match.first + " (" + std::to_string(match.second).substr(0, 4) + ")";
            cv::putText(result_img, label, 
                       cv::Point(face_info.x[0], face_info.y[0] - 10), 
                       cv::FONT_HERSHEY_SIMPLEX, 0.6, cv::Scalar(0, 255, 0), 2);
            
            // 畫關鍵點
            for (int j = 0; j < 5; j++) {
                cv::circle(result_img, 
                          cv::Point(face_info.landmark[2*j], face_info.landmark[2*j+1]), 
                          3, cv::Scalar(0, 0, 255), -1);
            }
        }
        
        // 保存結果圖片到 results 目錄
        if (config.save_detection_boxes) {
            std::string result_filename = "recognition_result.jpg";
            std::string full_result_path = config.getResultPath(result_filename);
            cv::imwrite(full_result_path, result_img);
            std::cout << "Recognition result saved as: " << full_result_path << std::endl;
        }
        
    } else if (command == "list") {
        auto persons = db.getAllPersons();
        std::cout << "Database contains " << persons.size() << " person(s):" << std::endl;
        for (const auto& person : persons) {
            std::cout << "- " << person.name << " (image: " << person.image_path 
                      << ", confidence: " << person.confidence << ")" << std::endl;
        }
        
    } else if (command == "remove" && argc == arg_start + 2) {
        std::string name = argv[arg_start + 1];
        if (db.removePerson(name)) {
            std::cout << "Successfully removed " << name << " from database." << std::endl;
        }
        
    } else {
        printUsage();
        return -1;
    }
    
    return 0;
}
