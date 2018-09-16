#pragma once

#include <iostream>
#include <math.h>

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#include <include/def.hpp>

class Process
{
private:

public:
    int thresh_simplify = 15;
    cv::Point gap_pos_left;
    cv::Point gap_pos_right;

public:
    Process();
    void simplify_data(std::vector<cv::Point> &points);
};
