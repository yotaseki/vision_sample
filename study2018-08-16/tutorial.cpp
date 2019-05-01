#include <stdio.h>
#include <opencv2/opencv.hpp>

struct mousePara{
    int x,y,event,flags;
};

void mouse_call_back(int event, int x, int y, int flags, void* userdata)
{
    mousePara *ptr = static_cast<mousePara*> (userdata);
    ptr->x = x;
    ptr->y = y;
    ptr->event = event;
    ptr->flags = flags;
}

int main(void){
    // 画像の読み込み
    cv::Mat img = cv::imread("images/image002.png");
    // 画像をコピーする
    cv::Mat copy = img.clone();
    // コピーした画像の表示
    cv::imshow("dialog", copy);
    // マウスイベントの呼び出し
    mousePara m;
    cv::setMouseCallback("dialog",mouse_call_back,&m);
    // 保存用
    int B_memo=0;
    int G_memo=0;
    int R_memo=0;
    // 無限ループ
    while(1)
    {
        int k = cv::waitKey(1);
        if(m.event==cv::EVENT_LBUTTONUP){ // 左クリックしたらここに入る
            // クリックした座標(m.x, m.y)を出力
            printf("m.x:%d, m.y:%d\n",m.x, m.y);
            // クリックした座標の画素を取得
            int width = copy.cols;
            int B = copy.data[(m.y*width + m.x)*copy.elemSize()+0];
            int G = copy.data[(m.y*width + m.x)*copy.elemSize()+1];
            int R = copy.data[(m.y*width + m.x)*copy.elemSize()+2];
            printf("B:%d, G:%d, R:%d \n",B,G,R);
            // 画像をリセット
            cv::Mat copy = img.clone();
            // 画像を表示
            cv::imshow("dialog", copy);
            // 保存用の変数にコピー
            B_memo = B;
            G_memo = G;
            R_memo = R;
        }
        if(k > 0){ //ボタンを押したら抜ける
            break; 
        }
    }
    // テキストに保存する
    FILE *fp;
    char str[256];
    fp = fopen("color_table.txt", "w");
    if(fp == NULL){
        printf("Failed file open.\n");
        return -1;
    }
    fprintf(fp, "%d %d %d \n",B_memo, G_memo, R_memo);
    fclose(fp);
    //表示されている全ての画像を閉じる
    cv::destroyAllWindows(); 
    return 0;
}


