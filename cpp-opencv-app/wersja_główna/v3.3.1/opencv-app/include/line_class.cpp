#include <string>
#include <vector>
#include "opencv2/opencv.hpp"

#include "line_class.hpp"

void LineDetector::applyBlur(cv::Mat &input, cv::Mat &output)
{
    cv::GaussianBlur(input, output, cv::Size(3,3), 0, 0);
}

void LineDetector::edgeDetect(cv::Mat &input, cv::Mat &output)
{
  cv::Mat kernel;
  cv::Point anchor;

  anchor = cv::Point(-1, -1);
  kernel = cv::Mat(1, 3, CV_32F);
  kernel.at<float>(0, 0) = -1;
  kernel.at<float>(0, 1) = 0;
  kernel.at<float>(0, 2) = 1;

  cv::threshold(input, input, 30, 255, cv::THRESH_BINARY);
  cv::filter2D(input, output, -1, kernel, anchor, 0, cv::BORDER_DEFAULT);
}

void LineDetector::applyMask(cv::Mat &input, cv::Mat &output)
{
    cv::Mat mask = cv::Mat::zeros(cv::Size(640, 360), CV_8UC1);
    cv::Point points[4] =
    {
        cv::Point(10, 340),
        cv::Point(100, 100),
        cv::Point(540, 100),
        cv::Point(630, 340)
    };

    cv::fillConvexPoly(mask, points, 4, cv::Scalar(255, 0, 0));
    cv::bitwise_and(input, mask, output);
}

void LineDetector::detectLines(cv::Mat &input, cv::vector<cv::Vec4i> &output_lines)
{
    HoughLinesP(input, output_lines, 1, CV_PI/180*10, 20, 30, 10);
}

void LineDetector::drawLines(cv::vector<cv::Vec4i> &input_lines, cv::Mat &output)
{
    output = cv::Mat::zeros(380, 640, CV_8UC1);

    for( size_t i = 0; i < input_lines.size(); i++ )
        {
            line(output,
                 cv::Point(input_lines[i][0], input_lines[i][1]),
                 cv::Point(input_lines[i][2], input_lines[i][3]),
                 cv::Scalar(255, 0, 0),
                 3,
                 8);
        }
}



