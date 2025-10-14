#include "face_database.h"
#include <algorithm>

FaceDatabase::FaceDatabase(const std::string& db_file) : db_filename(db_file) {
    loadFromFile();
}

FaceDatabase::~FaceDatabase() {
    saveToFile();
}

bool FaceDatabase::addPerson(const std::string& name, const std::string& image_path, 
                             const std::vector<float>& feature, float confidence) {
    if (feature.size() != 128) {
        std::cerr << "Error: Feature vector must be 128 dimensions!" << std::endl;
        return false;
    }
    
    // 檢查是否已存在同名人員，如果存在則更新
    for (auto& person : database) {
        if (person.name == name) {
            person.image_path = image_path;
            person.feature = feature;
            person.confidence = confidence;
            std::cout << "Updated person: " << name << std::endl;
            return saveToFile();
        }
    }
    
    // 添加新人員
    PersonFeature new_person;
    new_person.name = name;
    new_person.image_path = image_path;
    new_person.feature = feature;
    new_person.confidence = confidence;
    
    database.push_back(new_person);
    std::cout << "Added new person: " << name << std::endl;
    return saveToFile();
}

std::pair<std::string, float> FaceDatabase::searchPerson(const std::vector<float>& feature, 
                                                          float threshold) {
    if (feature.size() != 128) {
        std::cerr << "Error: Feature vector must be 128 dimensions!" << std::endl;
        return {"Unknown", 0.0};
    }
    
    if (database.empty()) {
        return {"Unknown", 0.0};
    }
    
    std::string best_match = "Unknown";
    float best_similarity = 0.0;
    
    for (const auto& person : database) {
        float similarity = calcSimilarity(feature, person.feature);
        if (similarity > best_similarity && similarity >= threshold) {
            best_similarity = similarity;
            best_match = person.name;
        }
    }
    
    return {best_match, best_similarity};
}

std::vector<PersonFeature> FaceDatabase::getAllPersons() {
    return database;
}

bool FaceDatabase::saveToFile() {
    std::ofstream file(db_filename);
    if (!file.is_open()) {
        std::cerr << "Error: Cannot open file for writing: " << db_filename << std::endl;
        return false;
    }
    
    file << database.size() << std::endl;
    for (const auto& person : database) {
        file << person.name << "|" << person.image_path << "|" << person.confidence;
        for (float f : person.feature) {
            file << "|" << f;
        }
        file << std::endl;
    }
    
    file.close();
    return true;
}

bool FaceDatabase::loadFromFile() {
    std::ifstream file(db_filename);
    if (!file.is_open()) {
        std::cout << "Database file not found, starting with empty database." << std::endl;
        return true; // 不是錯誤，只是文件不存在
    }
    
    database.clear();
    
    size_t count;
    file >> count;
    file.ignore(); // 忽略換行符
    
    for (size_t i = 0; i < count; i++) {
        std::string line;
        std::getline(file, line);
        
        std::stringstream ss(line);
        std::string token;
        std::vector<std::string> tokens;
        
        while (std::getline(ss, token, '|')) {
            tokens.push_back(token);
        }
        
        if (tokens.size() >= 131) { // name + image_path + confidence + 128 features
            PersonFeature person;
            person.name = tokens[0];
            person.image_path = tokens[1];
            person.confidence = std::stof(tokens[2]);
            person.feature.resize(128);
            
            for (int j = 0; j < 128; j++) {
                person.feature[j] = std::stof(tokens[3 + j]);
            }
            
            database.push_back(person);
        }
    }
    
    file.close();
    std::cout << "Loaded " << database.size() << " persons from database." << std::endl;
    return true;
}

bool FaceDatabase::removePerson(const std::string& name) {
    auto it = std::remove_if(database.begin(), database.end(),
                             [&name](const PersonFeature& person) {
                                 return person.name == name;
                             });
    
    if (it != database.end()) {
        database.erase(it, database.end());
        std::cout << "Removed person: " << name << std::endl;
        return saveToFile();
    }
    
    std::cout << "Person not found: " << name << std::endl;
    return false;
}

void FaceDatabase::clear() {
    database.clear();
    saveToFile();
}

size_t FaceDatabase::size() const {
    return database.size();
}

float FaceDatabase::calcSimilarity(const std::vector<float>& f1, const std::vector<float>& f2) {
    if (f1.size() != f2.size()) return 0.0;
    
    float sim = 0.0;
    for (size_t i = 0; i < f1.size(); i++) {
        sim += f1[i] * f2[i];
    }
    return sim;
}
