#pragma once

#include <iostream>

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#include <include/def.hpp>

class Process
{
private:

public:
    cv::Point gap_pos_left;
    cv::Point gap_pos_right;

public:
    Process();
};
