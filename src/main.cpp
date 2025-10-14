#include <vector>
#include <iostream>
#include <opencv2/opencv.hpp>
#include "arcface.h"
#include "mtcnn.h"
#include "face_database.h"
#include "config.h"

using namespace cv;
using namespace std;

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
    cout << "Face Recognition Database Demo" << endl;
    cout << "Usage:" << endl;
    cout << "  ./main [-c config.json] <command> [args...]" << endl;
    cout << "Commands:" << endl;
    cout << "  register <name> <image_path>  - Register a new person" << endl;
    cout << "  recognize <image_path>        - Recognize person in image" << endl;
    cout << "  list                          - List all registered persons" << endl;
    cout << "  remove <name>                 - Remove a person from database" << endl;
    cout << "Options:" << endl;
    cout << "  -c config.json               - Specify config file (default: config.json)" << endl;
    cout << "Example:" << endl;
    cout << "  ./main register \"John\" \"john.jpg\"" << endl;
    cout << "  ./main -c production.json recognize \"unknown.jpg\"" << endl;
}

int main(int argc, char* argv[])
{
    if (argc < 2) {
        printUsage();
        return -1;
    }
    
    // 解析命令列參數
    string config_file = "config.json";  // 預設配置檔案
    int arg_start = 1;  // 命令開始的參數索引
    
    // 檢查是否指定了配置檔案
    if (argc >= 3 && string(argv[1]) == "-c") {
        if (argc < 4) {
            printUsage();
            return -1;
        }
        config_file = argv[2];
        arg_start = 3;  // 命令從第3個參數開始
    }
    
    string command = argv[arg_start];
    
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
        string name = argv[arg_start + 1];
        string image_path = argv[arg_start + 2];
        
        // 讀取圖片
        Mat img = imread(image_path);
        if (img.empty()) {
            cerr << "Error: Cannot read image " << image_path << endl;
            return -1;
        }
        
        // 轉換為 ncnn 格式
        ncnn::Mat ncnn_img = ncnn::Mat::from_pixels(img.data, ncnn::Mat::PIXEL_BGR, img.cols, img.rows);
        
        // 檢測人臉
        vector<FaceInfo> results = detector.Detect(ncnn_img);
        if (results.empty()) {
            cerr << "Error: No face detected in image " << image_path << endl;
            return -1;
        }
        
        cout << "Detected " << results.size() << " face(s), using the first one." << endl;
        
        // 預處理人臉
        ncnn::Mat face = preprocess(ncnn_img, results[0]);
        
        // 提取特徵
        vector<float> feature = arc.getFeature(face);
        
        // 添加到數據庫
        if (db.addPerson(name, image_path, feature, results[0].score)) {
            cout << "Successfully registered " << name << " with confidence " << results[0].score << endl;
            
            // 保存檢測到的人臉
            cv::Mat face_img = ncnn2cv(face);
            string face_filename = name + "_face.jpg";
            cv::imwrite(face_filename, face_img);
            cout << "Face image saved as: " << face_filename << endl;
        } else {
            cerr << "Error: Failed to register " << name << endl;
            return -1;
        }
        
    } else if (command == "recognize" && argc == arg_start + 2) {
        string image_path = argv[arg_start + 1];
        
        // 讀取圖片
        Mat img = imread(image_path);
        if (img.empty()) {
            cerr << "Error: Cannot read image " << image_path << endl;
            return -1;
        }
        
        // 轉換為 ncnn 格式
        ncnn::Mat ncnn_img = ncnn::Mat::from_pixels(img.data, ncnn::Mat::PIXEL_BGR, img.cols, img.rows);
        
        // 檢測人臉
        vector<FaceInfo> results = detector.Detect(ncnn_img);
        if (results.empty()) {
            cerr << "Error: No face detected in image " << image_path << endl;
            return -1;
        }
        
        cout << "Detected " << results.size() << " face(s):" << endl;
        
        // 在圖片上標註結果
        Mat result_img = img.clone();
        
        for (size_t i = 0; i < results.size(); i++) {
            // 預處理人臉
            ncnn::Mat face = preprocess(ncnn_img, results[i]);
            
            // 提取特徵
            vector<float> feature = arc.getFeature(face);
            
            // 在數據庫中搜索
            auto match = db.searchPerson(feature, 0.6);
            
            cout << "Face " << (i+1) << ": " << match.first << " (similarity: " << match.second << ")" << endl;
            
            // 在圖片上畫框和標籤
            FaceInfo& face_info = results[i];
            cv::rectangle(result_img, 
                         cv::Point(face_info.x[0], face_info.y[0]), 
                         cv::Point(face_info.x[1], face_info.y[1]), 
                         cv::Scalar(0, 255, 0), 2);
            
            // 添加文字標籤
            string label = match.first + " (" + to_string(match.second).substr(0, 4) + ")";
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
        
        // 保存結果圖片
        string result_filename = "recognition_result.jpg";
        cv::imwrite(result_filename, result_img);
        cout << "Recognition result saved as: " << result_filename << endl;
        
    } else if (command == "list") {
        auto persons = db.getAllPersons();
        cout << "Database contains " << persons.size() << " person(s):" << endl;
        for (const auto& person : persons) {
            cout << "- " << person.name << " (image: " << person.image_path 
                 << ", confidence: " << person.confidence << ")" << endl;
        }
        
    } else if (command == "remove" && argc == arg_start + 2) {
        string name = argv[arg_start + 1];
        if (db.removePerson(name)) {
            cout << "Successfully removed " << name << " from database." << endl;
        }
        
    } else {
        printUsage();
        return -1;
    }
    
    return 0;
}
