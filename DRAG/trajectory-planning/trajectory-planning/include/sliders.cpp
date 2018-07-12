#include "sliders.h"
using namespace std;
using namespace cv;

const int left_slider_max = 4000;
const int right_slider_max = 640;

int left_slider[5] = {2000,0,283,284,285};
int right_slider[5]= {360,360,360,360,360};

const int max_num = 50;
const int max_off = 200;
const int max_derive = Height;
const int max_pixel = 100;

int rect_slider[5]={40,10,20,Height,0};


void left_trackbar( int, void* )
{
    for(int i=0;i<5;i++)
   left_slider[i]=left_slider[i];


}
void right_trackbar(int, void*)
{
   for(int i=0;i<5;i++)
       right_slider[i]=right_slider[i];
}

void init_trackbars()
{
       namedWindow("Parametry", WINDOW_NORMAL);
       createTrackbar("predkosc","Parametry", &left_slider[0], left_slider_max, left_trackbar );
       createTrackbar( "kat","Parametry", &left_slider[1], left_slider_max, left_trackbar );

       //namedWindow("right border line", WINDOW_NORMAL);
      // for(int i=0;i<5;i++)
       //{

       //createTrackbar( std::to_string(i), "right border line", &right_slider[i], right_slider_max, right_trackbar );
       //}
}

void rect_trackbars( int, void* )
{
    for(int i=0;i<5;i++)
    rect_slider[i]=rect_slider[i];

    if(rect_slider[0]<8)
        rect_slider[0]=8;
    if(rect_slider[1]<8)
        rect_slider[1]=8;
}

void init_rect_trackbars(String name)
{


    createTrackbar("w", name, &rect_slider[0], max_num, rect_trackbars );
    createTrackbar("h", name, &rect_slider[1], max_num, rect_trackbars );
    createTrackbar("max pix", name, &rect_slider[2], max_pixel, rect_trackbars );
    createTrackbar("deriv point", name, &rect_slider[3], max_derive, rect_trackbars );
    createTrackbar("offset", name, &rect_slider[4], max_off, rect_trackbars );


}
void poly_fill(Mat& frame,const Scalar& col,spline_t left, spline_t right,Mat& result)
{

    Point fillp[1][2*480];
    const Point* ppt[1] = { fillp[0] };
    int npt[] = { 2*480 };
    Mat poly = Mat::zeros( 480, 640, CV_8UC3 );
    Mat dst;

    for(int i=0;i<480;i++){
        fillp[0][i].y=i;
        fillp[0][i].x=left.spline(i);
    }
    for(int i =0;i<480;i++){
        fillp[0][2*480-i-1].y=i;
        fillp[0][2*480-i-1].x=right.spline(i);
    }


    fillPoly( poly,ppt, npt, 1, Scalar( col[0], col[1],col[2]), 4 ); //fill poly
    addWeighted(frame, 0.6, poly, 0.4, 0.0, result);
}

void points_preview(vector<Point>points,Mat& frame,const Scalar& col){


    for (int i=0;i<(int)points.size();i++){
        Vec3b color;
        Point pom;
        for(int j=0;j<10;j++){
            pom.y = points[i].y;
            pom.x = points[i].x;



            color = frame.at<Vec3b>(pom);
            color.val[0] = col[0];
            color.val[1] = col[1];
            color.val[2] = col[2];
            frame.at<Vec3b>(pom) = color;
         }
    }

}
