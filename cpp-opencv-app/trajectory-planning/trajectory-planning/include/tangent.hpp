#pragma once 
#include <opencv2/opencv.hpp>
#include "spline.hpp"
#include "interpolation.h"
using namespace cv;

class tangent{
  public:
    double a;
    double b;
    double angle_deg;
    int spline_point;


    //poly3_interp poly3;
    //exp_interp exp_curve;

    tangent();
    void draw(Mat& frame,const Scalar& col);
    void angle(spline_t spl);
    void calculate(spline_t spl, int num);

    void calculate_poly2( poly2_interp poly2, int x);
    void calculate_exp(exp_interp exp_fit, int x);


};
