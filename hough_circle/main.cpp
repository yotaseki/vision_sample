#include "detector/detector_yolo.h"
#include "detector/hough.h"
#include <experimental/filesystem>
#include <iostream>
#include <sstream>
#include <iomanip>
#include <opencv2/opencv.hpp>
#include <chrono>

namespace fs = std::experimental::filesystem;
int detectBall(DetectorYOLO &dyolo, cv::Mat &src, struct BoundingBox &out);
void bbox2rect(struct BoundingBox &bbox, cv::Rect &rect);
void expandROI(cv::Rect &roi, int margin);


int img_w = 320;
int img_h = 240;

int main(int argc, char** argv){
    // initialize
    std::cout << "init" << std::endl;
    std::string color_table_path = "color_table.cnf";
    std::string cfg_path    = "v2tiny_4class.cfg";
    std::string weight_path = "v2tiny_4class_backup.weights";
    std::string img_dir     = argv[1];
    DetectorYOLO dyolo(img_w, img_h);
    ColorTable ct;
    ct.loadColorTable(color_table_path);
    HoughCircle hc;
    std::vector<int> threshold{80,20,20,20};
    dyolo.setupYOLO(cfg_path,weight_path,threshold);
    // detect
    fs::path output_dir = "crop_ball";
    fs::create_directory(output_dir);
    for(const fs::directory_entry &i : fs::directory_iterator(img_dir)){
        if(i.path().extension().string() == ".png"  || i.path().extension().string() == ".jpg"){
            // read image
            std::string img_path = i.path().string();
            std::cout << img_path << std::endl;
            cv::Mat img = cv::imread(img_path);
            // detect ball
            struct BoundingBox ball_v;
            std::cout <<"hough" << std::endl;
            if(detectBall(dyolo,img,ball_v) )continue;
            // color_table
            std::cout <<"hough" << std::endl;
            cv::Mat img_ycrcb;
            cv::cvtColor(img,img_ycrcb,CV_BGR2YCrCb);
            cv::Mat label_16u(img_ycrcb.rows, img_ycrcb.cols, CV_16UC1);
            ct.apply(img_ycrcb, label_16u);
            // detect_by_hough
            struct BoundingBox ball_hough;
            hc.process(label_16u, ball_v, ball_hough);
            // result
            cv::Mat res = img.clone();
            std::cout <<"dx=" <<ball_hough.x*img_w - ball_v.x*img_w << std::endl;
            std::cout <<"dy=" <<ball_hough.y*img_h - ball_v.y*img_h << std::endl;
            cv::Rect yolo;
            cv::Rect hough;
            bbox2rect(ball_v,yolo);
            bbox2rect(ball_hough,hough );
            // cv::rectangle(res,yolo,cv::Scalar(255,0,0),1);
            cv::circle( res, cv::Point(yolo.x+yolo.width*0.5, yolo.y+yolo.height*0.5), 1, cv::Scalar(255,0,0), -1, 8, 0 );
            // cv::rectangle(res,hough,cv::Scalar(0,0,255),1);
            cv::circle( res, cv::Point(hough.x+hough.width*0.5, hough.y+hough.height*0.5), 1, cv::Scalar(0,0,255), -1, 8, 0 );
            
            cv::Rect crop = yolo;
            expandROI(crop, 20);
            cv::Mat res_c(res,crop);
            cv::resize(res_c,res_c,cv::Size(200,200));
            cv::imshow("result", res_c);
            cv::waitKey(0);
        }
    }
    cv::destroyAllWindows();
    return 0;
}

int detectBall(DetectorYOLO &dyolo, cv::Mat &src, struct BoundingBox &out){
    dyolo.setImage(src);
    std::vector<object_pos> objects;
    std::vector<object_pos> white_line;
    dyolo.getObjects(objects, white_line);
    std::vector<struct BoundingBox> ball_v = dyolo.getBallBoundingBox();
    if(ball_v.size() > 0)
    {
        out.x = ball_v[0].x;
        out.y = ball_v[0].y;
        out.w = ball_v[0].w;
        out.h = ball_v[0].h;
        return 0;
    }
    else{
        return -1;
    }
}

void bbox2rect(struct BoundingBox &bbox, cv::Rect &rect){
    int left =  static_cast<int>((bbox.x - bbox.w/2.0)*img_w);
    int top =   static_cast<int>((bbox.y - bbox.h/2.0)*img_h);
    int right = static_cast<int>((bbox.x + bbox.w/2.0)*img_w);
    int bottom =static_cast<int>((bbox.y + bbox.h/2.0)*img_h);
    rect = cv::Rect(cv::Point(left,top),cv::Point(right,bottom));
}
void expandROI(cv::Rect &roi, int margin){
    cv::Rect roi_exp;
    roi_exp.x = std::max(0, roi.x - margin);
    roi_exp.y = std::max(0, roi.y - margin);
    roi_exp.width = std::min(roi.x + roi.width + (margin*2), img_w) - roi.x;
    roi_exp.height = std::min(roi.y + roi.height + (margin*2), img_h) - roi.y;
    roi = roi_exp;
}
