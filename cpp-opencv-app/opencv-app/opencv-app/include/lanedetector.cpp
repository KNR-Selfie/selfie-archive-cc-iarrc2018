#include "lanedetector.hpp"

LaneDetector::LaneDetector()
{

}

void LaneDetector::applyBlur(cv::Mat &input, cv::Mat &output)
{
    cv::GaussianBlur(input, output, cv::Size(3,3), 0, 0);
}

void LaneDetector::colorTransform()
{

}

void LaneDetector::edgeDetect()
{

}

void LaneDetector::detectLine(cv::Mat &input, std::vector<std::vector<cv::Point>> &output)
{
    std::vector<cv::Vec4i> hierarchy;
    int accuracy = 5;

    cv::findContours(input, output, hierarchy, CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE, cv::Point(0, 0));
    for (int i = 0; i < output.size(); i++)
    {
        cv::approxPolyDP(cv::Mat(output[i]),output[i], accuracy, false);
    }
}

void LaneDetector::drawData()
{

}
