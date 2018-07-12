#pragma once 
#include <opencv2/opencv.hpp>
#include "spline.hpp"
#include "interpolation.h"
#include <cmath>
#include "main.h"


using namespace cv;

class tangent{
  public:
    double a;
    double b;
    double angle_deg;
    int tangent_point;
    spline_t tangent_spline;

    //poly3_interp poly3;
    //exp_interp exp_curve;

    tangent();
    void draw(Mat& frame,const Scalar& col);
    void angle();
    void calculate(spline_t spl, int x);

    void calculate_poly2( poly2_interp poly2, int x);
    void calculate_exp(exp_interp exp_fit, int x);


};
