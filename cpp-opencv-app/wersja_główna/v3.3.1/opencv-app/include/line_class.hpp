#pragma once

#include "opencv2/opencv.hpp"
#include <opencv2/highgui/highgui.hpp>
#include<iostream>
#include <vector>
#include <string>

class LineDetector
{
    cv::Point CenterPoint;

    void calculateCenterPoint(cv::vector<cv::Vec4i> &lines);

public:
    void applyBlur(cv::Mat &input, cv::Mat &output);
    void edgeDetect(cv::Mat &input, cv::Mat &output_thresh, cv::Mat &output_edges, int &threshold_value);
    void applyMask(cv::Mat &input, cv::Mat &mask, cv::Mat &output);
    void detectLines(cv::Mat &input, cv::vector<cv::Vec4i> &output_lines);
    void drawLines(cv::vector<cv::Vec4i> &input_lines, cv::Mat &output);

    void get4corners(cv::vector<cv::Vec4i> &lines, cv::Mat &frame_lines, cv::vector<cv::Point> &lane_corners);
    void calculateDataToSend(cv::vector<cv::Point> &lane_corners, int &detected_middle_pos_near, int &detected_middle_pos_far, int &left_lane_angle_st, int &right_lane_angle_st, int8_t &flags_to_UART);
};
