#include <opencv2/opencv.hpp>
#include "spline.hpp"

using namespace cv;

const int left_slider_max = 600;
const int right_slider_max = 600;


int npt[] = { 2*480 };

Mat poly = Mat::zeros( 480, 640, CV_8UC3 );
Mat dst;


int left_slider[4] = {100,100,100,100};
int right_slider[4]= {500,500,500,500};

void left_trackbar( int, void* )
{
    for(int i=4;i<4;i++)
   left_slider[i]=left_slider[i];
}
void right_trackbar(int, void*)
{
   for(int i=0;i<4;i++)
       right_slider[i]=right_slider[i];
}

void init_trackbars()
{
       namedWindow("left border line", WINDOW_NORMAL);
       namedWindow("right border line", WINDOW_NORMAL);
       for(int i=0;i<4;i++){
       createTrackbar(std::to_string(i), "left border line", &left_slider[i], left_slider_max, left_trackbar );
       createTrackbar( std::to_string(i), "right border line", &right_slider[i], right_slider_max, right_trackbar );
       }
}
void poly_fill(Mat& frame,const Scalar& col,spline_t left, spline_t right,Point fillp[0][960],const Point* ppt[1])
{

    ppt[1] = { fillp[0] };
    for(int i=0;i<480;i++){
        fillp[0][i].y=i;
        fillp[0][i].x=left.spline(i);
    }
    for(int i =0;i<480;i++){
        fillp[0][2*480-i-1].y=i;
        fillp[0][2*480-i-1].x=right.spline(i);
    }

    poly = Mat::zeros( 480, 640, CV_8UC3 );
    fillPoly( poly,ppt, npt, 1, Scalar( col[0], col[1],col[2]), 4 ); //fill poly
    addWeighted(frame, 0.6, poly, 0.4, 0.0, dst);
}
