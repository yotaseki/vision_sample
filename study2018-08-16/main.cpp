#include <stdio.h>
#include <opencv2/opencv.hpp>

int max_int(int a, int b);
int min_int(int a, int b);

int main(void){
    // カラーテーブルの読み込み
    FILE *fp;
    fp = fopen("color_table.txt", "r");
    if(fp == NULL){
        printf("Failed file open.\n");
        return -1;
    }
    int B_memo, G_memo, R_memo;
    fscanf(fp, "%d %d %d",&B_memo, &G_memo, &R_memo);
    printf("B:%d, G:%d, R:%d \n",B_memo, G_memo, R_memo);
    
    // 画像の読み込み
    cv::Mat img = cv::imread("images/image001.png");
    int width = img.cols;
    int height= img.rows;
    // 端点の値
    int left = width;
    int top = img.rows;
    int right = 0;
    int bottom = 0;
    // 画像をラスタスキャン
    for(int y=0; y<height; y++){ // 列に対してループ
        for(int x=0; x<width; x++){ // 行に対してループ
            // (x, y)の画素を取得
            int B = img.data[(0 /* 0を消して, ここに入れる値を考えてください */)*img.elemSize()+0];
            int G = img.data[(0 /* 0を消して, ここに入れる値を考えてください */)*img.elemSize()+1];
            int R = img.data[(0 /* 0を消して, ここに入れる値を考えてください */)*img.elemSize()+2];
            if( (B==B_memo && G==G_memo) && R==R_memo ){ // 色が一致したら
                left   = min_int(left, x);
                right  = max_int(right, x);
                top    = min_int(top, y);
                bottom = max_int(bottom, y);
            }
        }
    }
    // 四角形を描画
    cv::rectangle(img,cv::Point(left,top),cv::Point(right,bottom), cv::Scalar(255,0,0),3);
    // 画像を表示
    cv::imshow("result", img);
    // キーを押すまで待機
    cv::waitKey(0);
    // 表示している画像を全て閉じる
    cv::destroyAllWindows();
    // 画像を保存
    cv::imwrite("result.png", img);
    return 0;
}

// 大きいほうの値を返す関数
int max_int(int a, int b){
    if(a > b){
        return a;
    }
    else{
        return b;
    }
}

// 小さいほうの値を返す関数
int min_int(int a, int b){
    if(a > b){
        return b;
    }
    else{
        return a;
    }
}
