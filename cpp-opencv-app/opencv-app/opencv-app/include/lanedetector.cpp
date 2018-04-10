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
    for (unsigned int i = 0; i < output.size(); i++)
    {
        cv::approxPolyDP(cv::Mat(output[i]),output[i], accuracy, false);
    }
}

void LaneDetector::drawData(cv::Mat &frame, std::vector<std::vector<cv::Point>> &data_points)
{
    for (unsigned int i = 0; i < data_points.size(); i++)
    {
            for (unsigned int j = 0; j<data_points[i].size(); j++)
            {
                cv::circle(frame, data_points[i][j], 3, cv::Scalar(0, 255, 255), cv::FILLED, cv::LINE_AA);
                if (j>0)
                    cv::line(frame, cv::Point(data_points[i][j - 1]), cv::Point(data_points[i][j]), cv::Scalar(0, 0, 255), 2);
            }
        }
}
