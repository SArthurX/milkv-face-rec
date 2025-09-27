#include <dlib/image_processing/frontal_face_detector.h>
#include <dlib/image_processing.h>
#include <dlib/image_io.h>
#include <dlib/opencv.h>
#include <opencv2/opencv.hpp>
#include <iostream>
#include <vector>
#include <cmath>

int main() {
    try {
        std::cout << "人臉檢測程式啟動..." << std::endl;
        
        dlib::frontal_face_detector detector = dlib::get_frontal_face_detector();

        dlib::matrix<dlib::rgb_pixel> img1, img2;
        dlib::load_image(img1, "test1.jpg");
        dlib::load_image(img2, "test2.jpg");

        std::cout << "圖片載入成功" << std::endl;

        auto dets1 = detector(img1);
        auto dets2 = detector(img2);

        std::cout << "圖片1偵測到 " << dets1.size() << " 個臉" << std::endl;
        std::cout << "圖片2偵測到 " << dets2.size() << " 個臉" << std::endl;

        if (dets1.empty() || dets2.empty()) {
            std::cout << "未偵測到臉部" << std::endl;
            return 0;
        }

        auto rect1 = dets1[0];
        auto rect2 = dets2[0];
        
        double center1_x = (rect1.left() + rect1.right()) / 2.0;
        double center1_y = (rect1.top() + rect1.bottom()) / 2.0;
        double center2_x = (rect2.left() + rect2.right()) / 2.0;
        double center2_y = (rect2.top() + rect2.bottom()) / 2.0;
        
        double size1 = rect1.width() * rect1.height();
        double size2 = rect2.width() * rect2.height();
        
        double center_distance = std::sqrt(std::pow(center1_x - center2_x, 2) + std::pow(center1_y - center2_y, 2));
        double size_ratio = std::abs(size1 - size2) / std::max(size1, size2);
        
        std::cout << "臉部中心距離: " << center_distance << std::endl;
        std::cout << "臉部大小差異比例: " << size_ratio << std::endl;
        
        if (center_distance < 50 && size_ratio < 0.3) {
            std::cout << "同一個人" << std::endl;
        } else {
            std::cout << "不同人" << std::endl;
        }

    } catch (std::exception& e) {
        std::cerr << "錯誤: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}
