#pragma once 
#include <opencv2/opencv.hpp>
#include "spline.hpp"

using namespace cv;

class tangent{
  public:
    double a;
    double b;
    double angle_deg;
    int spline_point;
    spline_t spline;

    tangent();
    void draw(Mat& frame,const Scalar& col);
    void angle(spline_t spl);
    void calculate(spline_t spl, int num);

};
