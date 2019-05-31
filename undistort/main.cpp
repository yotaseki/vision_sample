#include <stdio.h>
#include <iostream>
#include <fstream>
#include <vector>
#include <opencv2/opencv.hpp>

void getCameraRT(cv::Mat &rvec, cv::Mat &tvec, cv::Mat &RT);
void undistort_point(int x, int y, int width, int height, cv::Mat &cameraMatrix, cv::Mat &distCoeffs, int *rx, int *ry);
void get_chess_points3f(std::vector<cv::Point3f> &, int,int,int);
void detect_board(cv::Mat &src, cv::Size p_size, std::vector<cv::Point2f> &corners, int draw);

int main(int argc, char **argv){
    int board_col;
    int board_row;
    int board_size;
    printf("Input chess board cols:");
    scanf("%d",&board_col);
    printf("Input chess board rows:");
    scanf("%d",&board_row);
    printf("Input size of pattern:");
    scanf("%d",&board_size);
    int capture_device = 0;
    if(argc == 2){
        capture_device = atoi(argv[1]);
    }
    cv::VideoCapture cam;
    if(capture_device >= 0){
        cam.open(capture_device);
    }
    else{
        cam.open(0);
    }
    if(!cam.isOpened()){
        printf("device cannot open.");
        return -1;
    }
    std::vector<cv::Mat> caps;
    std::vector<std::vector<cv::Point2f> >  imgPoints;
    std::vector<std::vector<cv::Point3f> >  objPoints;
    cv::Mat frame;
    cv::Mat cameraMatrix;
    cv::Mat distCoeffs;
    int num=0;
    int undistort_flag = 0;
    std::vector<cv::Point3f> obj_p;
    get_chess_points3f(obj_p, board_col-1, board_row-1, board_size);
    while(cam.read(frame)){
        std::vector<cv::Point2f> corners;
        cv::Mat img = frame.clone();
        if(undistort_flag == 1){
            /* undistort */
            cv::Mat img_und;
            cv::undistort(frame,img_und,cameraMatrix,distCoeffs);
            /* solvepnp */
            detect_board(img,cv::Size(board_col-1, board_row-1),corners,1);
            if(corners.size() == obj_p.size()){
                cv::Mat rvec, tvec;
                cv::solvePnP(obj_p, corners, cameraMatrix, distCoeffs,rvec, tvec);
                if(true){
                    std::cout << " ----- " << std::endl;
                    cv::Mat RT;
                    getCameraRT(rvec,tvec,RT);
                    std::cout << RT << std::endl;
                }
                /* drawing */
                int l = 4 * board_size;
                std::vector<cv::Point3f> axis = {cv::Point3f(l,0,0),cv::Point3f(0,l,0),cv::Point3f(0,0,-l)};
                std::vector<cv::Point2f> imgPts;
                cv::projectPoints(axis, rvec, tvec, cameraMatrix, distCoeffs, imgPts);
                if(imgPts.size() == 3){
                    cv::line(img,corners[0],imgPts[0],cv::Scalar(255,0,0), 5);
                    cv::line(img,corners[0],imgPts[1],cv::Scalar(0,255,0), 5);
                    cv::line(img,corners[0],imgPts[2],cv::Scalar(0,0,255), 5);
                }
            }
            /* undistort_points */
            for(int cx=50; cx<img.cols-50; cx+=10){
                for(int cy=50; cy<img.rows-50; cy+=10){
                    float color_col = float(cx) / img.cols;
                    float color_row = float(cy) / img.rows;
                    cv::circle(img,cv::Point2d(cx,cy),2,cv::Scalar(0,255*color_row,255*color_col),-1);
                    int rx, ry;
                    undistort_point(cx,cy,img.cols,img.rows,cameraMatrix,distCoeffs,&rx,&ry );
                    cv::circle(img_und,cv::Point2d(rx,ry),2,cv::Scalar(0,255*color_row,255*color_col),-1);
                }
            }
            cv::imshow("undistort",img_und);
            /* Remap */
            /*
            cv::Mat newCameraMat = cv::getOptimalNewCameraMatrix(cameraMatrix,distCoeffs,img.size(),1);
            cv::Mat map1, map2, R;
            cv::initUndistortRectifyMap(cameraMatrix,distCoeffs,R,newCameraMat,img.size(),CV_32FC1,map1,map2);
            cv::Mat img_remap;
            cv::remap(frame,img_remap,map1,map2,cv::INTER_NEAREST);
            cv::imshow("remap",img_remap);
            */
        }
        else{
            detect_board(img,cv::Size(board_col-1, board_row-1),corners,1 );
        }
        cv::imshow("cam",img);
        const int key=cv::waitKey(1) & 0xff;
        if(key == 'q'){
            break;
        }
        else if(key == 'a'){
            if(corners.size() == obj_p.size()){
                cv::Mat m = frame.clone();
                caps.push_back(m);
                imgPoints.push_back(corners);
                objPoints.push_back(obj_p);
                num++;
                std::cout << "num:" << num <<std::endl;
            }
        }
        else if(key == 'c'){
            if(num > 0){
                cv::Mat camMat, dist, rvecs, tvecs;
                cv::calibrateCamera(objPoints, imgPoints, cv::Size(frame.cols,frame.rows), camMat, dist, rvecs, tvecs);
                int r = 100000;
                std::cout << "fx:"<< std::round(camMat.at<double>(0,0)*r/frame.cols)/r << std::endl;
                std::cout << "fy:"<< std::round(camMat.at<double>(1,1)*r/frame.rows)/r << std::endl;
                std::cout << "cx:"<< std::round(camMat.at<double>(0,2)*r/frame.cols)/r << std::endl;
                std::cout << "cy:"<< std::round(camMat.at<double>(1,2)*r/frame.cols)/r << std::endl;
                std::cout << "k0:"<< std::round(dist.at<double>(0,0)*r)/r << std::endl;
                std::cout << "k1:"<< std::round(dist.at<double>(0,1)*r)/r << std::endl;
                std::cout << "k2:"<< std::round(dist.at<double>(0,4)*r)/r << std::endl;
                std::cout << "k3:"<< std::round(dist.at<double>(0,5)*r)/r << std::endl;
                std::cout << "p1:"<< std::round(dist.at<double>(0,2)*r)/r << std::endl;
                std::cout << "p2:"<< std::round(dist.at<double>(0,3)*r)/r << std::endl;
                cameraMatrix = camMat.clone();
                distCoeffs = dist.clone();
                undistort_flag = 1;
                std::ofstream ofs("camera_param.txt");
                ofs << "fx:"<< std::round(camMat.at<double>(0,0)*r/frame.cols)/r << std::endl;
                ofs << "fy:"<< std::round(camMat.at<double>(1,1)*r/frame.rows)/r << std::endl;
                ofs << "cx:"<< std::round(camMat.at<double>(0,2)*r/frame.cols)/r << std::endl;
                ofs << "cy:"<< std::round(camMat.at<double>(1,2)*r/frame.cols)/r << std::endl;
                ofs << "k0:"<< std::round(dist.at<double>(0,0)*r)/r << std::endl;
                ofs << "k1:"<< std::round(dist.at<double>(0,1)*r)/r << std::endl;
                ofs << "k2:"<< std::round(dist.at<double>(0,4)*r)/r << std::endl;
                ofs << "k3:"<< std::round(dist.at<double>(0,5)*r)/r << std::endl;
                ofs << "p1:"<< std::round(dist.at<double>(0,2)*r)/r << std::endl;
                ofs << "p2:"<< std::round(dist.at<double>(0,3)*r)/r << std::endl;
            }
        }
    }
    for(int i=0;i<caps.size();i++){
        char fn[256];
        sprintf(fn,"board%03d.png",i);
        cv::imwrite(fn,caps[i]);
    }
    return 0;
}

void getCameraRT(cv::Mat &rvec, cv::Mat &tvec, cv::Mat &RT){
    cv::Mat camera_rt = cv::Mat::zeros(4,4,CV_64FC1);
    cv::Mat Rotate;
    cv::Rodrigues(rvec,Rotate);
    Rotate.copyTo(camera_rt(cv::Rect(0,0,3,3)));
    tvec.copyTo(camera_rt(cv::Rect(3,0,1,3)));
    RT = camera_rt.clone();
}

void undistort_point(int xi, int yi, int width, int height,cv::Mat &cameraMatrix, cv::Mat &distCoeffs, int *rx, int *ry){
    float x = float(xi);
    float y = float(yi);
    cv::Mat src(1, 1, CV_64FC2);
    std::vector<cv::Point2d> dst(1);

    src.at<cv::Vec2d>(0, 0)[0] = x;
    src.at<cv::Vec2d>(0, 0)[1] = y;
    
    cv::Mat R,P;
    cv::undistortPoints(src, dst, cameraMatrix, distCoeffs,R,P=cameraMatrix);
    //std::cout << " ------ " << std::endl;
    //std::cout << "x:" << x << " -> ux:" << dst[0].x << std::endl;
    //std::cout << "y:" << y << " -> uy:" << dst[0].y << std::endl;
    *rx = int(dst[0].x);
    *ry = int(dst[0].y);
}

void get_chess_points3f(std::vector<cv::Point3f> &objPoint, int cols, int rows, int patternSize){
    for(int y=0; y<rows; y++){
        for(int x=0; x<cols; x++){
            cv::Point3f p(x*patternSize, y*patternSize, 0.0f);
            objPoint.push_back(p);
        }
    }
}

void detect_board(cv::Mat &src, cv::Size p_size, std::vector<cv::Point2f> &corners, int draw=0){
    cv::Mat gray;
    cv::cvtColor(src,gray,CV_BGR2GRAY);
    bool ret = cv::findChessboardCorners(gray,p_size, corners,CV_CALIB_CB_ADAPTIVE_THRESH + CV_CALIB_CB_NORMALIZE_IMAGE + CV_CALIB_CB_FAST_CHECK);
    if(ret){
        cv::cornerSubPix(gray, corners, cv::Size(11, 11), cv::Size(-1, -1),cv::TermCriteria(CV_TERMCRIT_EPS + CV_TERMCRIT_ITER, 30, 0.1));
    }
    if(draw){
        cv::drawChessboardCorners(src, p_size, cv::Mat(corners), ret);
    }
}
