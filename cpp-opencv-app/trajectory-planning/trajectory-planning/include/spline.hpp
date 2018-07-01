#pragma once

#include <stdio.h>
#include <cstdio>
#include <vector>
#include <iostream>
#include "spline.h"
#include <opencv2/opencv.hpp>


using namespace cv;
using namespace tk;

class spline_t
{
public:
    tk::spline spline;
    std :: vector<double> X;
    std :: vector<double> Y;

    spline_t();
    void add_point();
    void delete_point();
    void fit_vector(int points_num);
    void set_point_value(int num,double x, double y);

    void set_spline(std::vector<Point> vec); //wektor musi być posortowany po x
    void draw(cv::Mat& frame,const cv::Scalar& col);
};
