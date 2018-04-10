#pragma once

#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"

class LaneDetector
{
public:
    LaneDetector();
    void applyBlur();
    void colorTransform();
    void edgeDetect();
    void detectLine(cv::Mat &input, std::vector<std::vector<cv::Point>> &output);
    void drawData();
};
