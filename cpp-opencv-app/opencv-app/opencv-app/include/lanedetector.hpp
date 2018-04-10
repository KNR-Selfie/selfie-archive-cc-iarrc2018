#pragma once

#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"

class LaneDetector
{
public:
    LaneDetector();
    void applyBlur(cv::Mat &input, cv::Mat &output);
    void colorTransform();
    void edgeDetect();
    void detectLine(cv::Mat &input, std::vector<std::vector<cv::Point>> &output);
    void drawData(cv::Mat &frame, std::vector<std::vector<cv::Point>> &data_points);
};
