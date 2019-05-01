#include "detector/detector_yolo.h"
#include <opencv2/opencv.hpp>

int main(int argc, char** argv){
    // initialize
    int img_w = 640;
    int img_h = 480;
    std::cout << "init" << std::endl;
    std::string cfg_name = argv[1];
    std::string weight_name = argv[2];
    std::string img_name = argv[3];
    std::vector<int> threshold{20,20,20,20};
    DetectorYOLO dyolo(img_w, img_h);
    std::cout << "CONFIG: "<< cfg_name << std::endl;
    std::cout << "WEIGHT: "<< weight_name << std::endl;
    dyolo.setupYOLO(cfg_name,weight_name,threshold);
    // detect
    std::cout << "detect" << std::endl;
    cv::Mat img = cv::imread(img_name);
    dyolo.setImage(img);
    std::vector<object_pos> objects;
    std::vector<object_pos> white_line;
    dyolo.getObjects(objects, white_line);
    std::vector<struct BoundingBox> ball_v = dyolo.getBallBoundingBox();
    // draw
    std::cout << "draw" << std::endl;
    cv::Mat img_draw = img.clone();
    int X = ball_v[0].x * img_w;
    int Y = ball_v[0].y * img_h;
    int W = ball_v[0].w * img_w;
    int H = ball_v[0].h * img_h;
    cv::rectangle(img_draw,cv::Point(X-W/2, Y-H/2),cv::Point(X+W/2,Y+H/2),(255,0,0),4);
    // cut
    std::cout << "cut" << std::endl;
    cv::Mat img_cut(img,cv::Rect(X-W/2, Y-H/2,W,H));
    // show
    cv::imshow("draw",img_draw);
    cv::imshow("cut",img_cut);
    cv::waitKey(0);
    cv::destroyAllWindows();
    
    return 0;
}

void detect(){
}
