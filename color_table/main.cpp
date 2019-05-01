#include <iostream>
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
    std::string fn = argv[1]; // image name
    cv::Mat src = cv::imread(fn);
    cv::cvtColor(src,src,CV_BGR2YCrCb);
    cv::Mat labelimage;
    make_labelimage(src,labelimage);
    cv::Mat dst;
    separate_labelimage(COLOR_GREEN,labelimage,dst);
    cv::imshow("result",dst);
    cv::waitKey(0);
    cv::destroyAllWindows();
}

void make_labelimage(cv::Mat &src, cv::Mat &dst)
{
  ColorTable color_table;
  color_table.loadColorTable(std::string("color_table.cnf"));
  cv::Mat labeling_image(src.rows, src.cols, CV_16UC1);
  color_table.apply(src, labeling_image);
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
