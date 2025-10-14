#ifndef FACE_DATABASE_H
#define FACE_DATABASE_H

#include <vector>
#include <string>
#include <fstream>
#include <iostream>
#include <sstream>

struct PersonFeature {
    std::string name;           // 人名
    std::string image_path;     // 圖片路径
    std::vector<float> feature; // 128維特徵向量
    float confidence;           // 置信度
};

class FaceDatabase {
public:
    FaceDatabase(const std::string& db_file = "../result/face_database.txt");
    ~FaceDatabase();
    
    // 添加新的人臉特徵到數據庫
    bool addPerson(const std::string& name, const std::string& image_path, 
                   const std::vector<float>& feature, float confidence = 1.0);
    
    // 在數據庫中搜索最相似的人臉
    std::pair<std::string, float> searchPerson(const std::vector<float>& feature, 
                                                float threshold = 0.6);
    
    // 獲取所有人員列表
    std::vector<PersonFeature> getAllPersons();
    
    // 保存數據庫到文件
    bool saveToFile();
    
    // 從文件加載數據庫
    bool loadFromFile();
    
    // 刪除指定人員
    bool removePerson(const std::string& name);
    
    // 清空數據庫
    void clear();
    
    // 獲取數據庫大小
    size_t size() const;

private:
    std::vector<PersonFeature> database;
    std::string db_filename;
    
    // 計算相似度
    float calcSimilarity(const std::vector<float>& f1, const std::vector<float>& f2);
};

#endif // FACE_DATABASE_H
