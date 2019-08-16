#include <iostream>
#include <sstream>
#include <opencv2/opencv.hpp>
#include "detector/color_table.h"

void make_labelimage(cv::Mat &src, cv::Mat &dst);
void separate_labelimage(int colorflag, cv::Mat &labelimage, cv::Mat &dst);
enum Col_def_T {
    COLOR_BALL = 0,
    COLOR_YELLOW,
    COLOR_BLUE,
    COLOR_BLACK,
    COLOR_MAGENTA,
    COLOR_CYAN,
    COLOR_WHITE,
    COLOR_GREEN,
    COLOR_BLACK_ON_GREEN,
    COLOR_BLACK_ON_GREEN_ERASE_V_LINE
};

int main(int argc ,char** argv){
    if(argc < 1){
        std::cout << "usage: ./color_table <image_files>" << std::endl;
    }
    for(int i=1;i<argc;i++){
        std::string fn = argv[i]; // image name
        cv::Mat src = cv::imread(fn);
        if(src.empty()){
            std::cout << "failed open:"<< fn << std::endl;
            continue;
        }
        std::stringstream sstr;
        sstr << fn << "_out.png";
        std::cout << sstr.str() <<std::endl;
        cv::Mat labelimage;
        make_labelimage(src,labelimage);
        cv::Mat dst;
        separate_labelimage(COLOR_WHITE,labelimage,dst);
        cv::imwrite(sstr.str(),dst);
    }
}

void make_labelimage(cv::Mat &src, cv::Mat &dst)
{
    cv::Mat src_ycrcb;
    cv::cvtColor(src,src_ycrcb,CV_BGR2YCrCb);
    ColorTable color_table;
    color_table.loadColorTable(std::string("color_table.cnf"));
    cv::Mat labeling_image(src_ycrcb.rows, src_ycrcb.cols, CV_16UC1);
    color_table.apply(src_ycrcb, labeling_image);
    dst = labeling_image;
}

void separate_labelimage(int colorflag, cv::Mat &labelimage, cv::Mat &dst)
{
    cv::Mat mat(labelimage.rows, labelimage.cols, CV_8UC1);
    for(int x=0;x<labelimage.cols;x++)
    {
        for(int y=0;y<labelimage.rows;y++)
        {
            if( 1<<colorflag & labelimage.data[y * labelimage.cols + x])
            {
                mat.data[y * labelimage.cols + x] = 255;
            }
            else{
                mat.data[y * labelimage.cols + x] = 0;
            }
        }
    }
    dst = mat;
}
