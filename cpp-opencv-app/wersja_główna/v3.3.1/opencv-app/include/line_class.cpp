#include <string>
#include <vector>
#include "opencv2/opencv.hpp"

#include "line_class.hpp"

//private:

void LineDetector::calculateCenterPoint(cv::vector<cv::Vec4i> &lines)
{
    int x_pos = 0;
    int y_pos = 0;

    for( size_t i = 0; i < lines.size(); i++ )
    {
        //x_pos += lines[i][0];
        //x_pos += lines[i][2];

        //y_pos += lines[i][1];
        //y_pos += lines[i][3];
    }

    //x_pos = x_pos / (2*lines.size());
    //y_pos = y_pos / (2*lines.size());

    CenterPoint = cv::Point(x_pos, y_pos);
}


//public:

void LineDetector::applyBlur(cv::Mat &input, cv::Mat &output)
{
    cv::GaussianBlur(input, output, cv::Size(3,3), 0, 0);
}

void LineDetector::edgeDetect(cv::Mat &input, cv::Mat &output_thresh, cv::Mat &output_edges, int &threshold_value)
{
  cv::Mat kernel;
  cv::Point anchor;

  anchor = cv::Point(-1, -1);
  kernel = cv::Mat(1, 3, CV_32F);
  kernel.at<float>(0, 0) = -1;
  kernel.at<float>(0, 1) = 0;
  kernel.at<float>(0, 2) = 1;

  cv::threshold(input, output_thresh, threshold_value, 255, cv::THRESH_BINARY_INV);
  cv::filter2D(output_thresh, output_edges, -1, kernel, anchor, 0, cv::BORDER_DEFAULT);
}

void LineDetector::applyMask(cv::Mat &input, cv::Mat &mask, cv::Mat &output)
{
    cv::bitwise_and(input, mask, output);
}

void LineDetector::detectLines(cv::Mat &input, cv::vector<cv::Vec4i> &output_lines)
{
    HoughLinesP(input, output_lines, 1, CV_PI/180*10, 5, 25, 10);
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

void LineDetector::get4corners(cv::vector<cv::Vec4i> &lines, cv::Mat &frame_lines, cv::vector<cv::Point> &lane_corners)
{
    calculateCenterPoint(lines);

    cv::circle(frame_lines, CenterPoint, 10, cv::Scalar(255,0,0), 1, CV_FILLED, 0);
}

void LineDetector::calculateDataToSend(cv::vector<cv::Point> &lane_corners, int &detected_middle_pos_near, int &detected_middle_pos_far, int &left_lane_angle_st, int &right_lane_angle_st, int8_t &flags_to_UART)
{

}

