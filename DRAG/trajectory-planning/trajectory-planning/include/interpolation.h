#pragma once

#include <opencv2/opencv.hpp>
#include "include/vvector.h"
#include <vector>
#include <iostream>

using namespace std;
using namespace cv;

class poly2_interp{

public:
    double a;
    double b;
    double c;

    poly2_interp();
    void calculate_coef(vector<Point> pts);
    void draw(Mat& frame,const Scalar& col);

};

class poly3_interp{

public:
    double a;
    double b;
    double c;
    double d;

    poly3_interp();
    void calculate_3coef(vector<Point> pts);
    void draw(Mat& frame,const Scalar& col);

};

class exp_interp{

public:
    double A;
    double B;

    exp_interp();
    void calculate_exp(vector<Point> pts,uint32_t len);
    void draw(Mat& frame,const Scalar& col);


};




