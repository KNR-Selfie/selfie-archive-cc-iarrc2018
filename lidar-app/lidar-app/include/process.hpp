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
    std::vector<cv::Point> rejected_points;

public:
    int max_dist = 30;
    int thresh_simplify = 7;
    std::vector<cv::Point> left_points;
    std::vector<cv::Point> right_points;
    cv::Point gap_pos_left;
    cv::Point gap_pos_right;

public:
    Process();
    void simplify_data(std::vector<cv::Point> &points);
    void split_poins(std::vector<cv::Point> &points);
    void split_poins_equally(std::vector<cv::Point> &points);
    void draw_data(cv::Mat &out);
};
