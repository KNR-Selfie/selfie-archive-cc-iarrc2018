#pragma once

#include "opencv2/opencv.hpp"
#include <opencv2/highgui/highgui.hpp>
#include<iostream>
#include <vector>
#include <string>

class LineDetector
{

    //cv::Point CenterPointNear = cv::Point(320, 150);
    //cv::Point CenterPointFar = cv::Point(320, 230);

    //int Row4Top = 180;
    //int Row4Bottom = 240;

    //void calculateCenterPoint(cv::vector<cv::Vec4i> &lines);
    //void calculate_bisector(int &left_lane_angle_st, int &right_lane_angle_st);

    struct Punkt
    {
        cv::Point coordinates;
        float slope;
    };

    cv::vector<Punkt> all_points;

    void calculate_slopes(cv::vector<cv::Vec4i> &all_lines);
    void average_slope(cv::vector<Punkt> punkty);

public:
    void applyBlur(cv::Mat &input, cv::Mat &output);
    void edgeDetect(cv::Mat &input, cv::Mat &output_thresh, cv::Mat &output_edges, int &threshold_value);
    void applyMask(cv::Mat &input, cv::Mat &mask, cv::Mat &output);
    void detectLines(cv::Mat &input, cv::vector<cv::Vec4i> &output_lines);
    void drawLines(cv::vector<cv::Vec4i> &input_lines, cv::Mat &output);
    void writePoints();

/*
    void get2points(cv::vector<cv::Vec4i> &lines, cv::Mat &frame_lines, cv::vector<cv::Point> &lane_corners);
    void calculateDataToSend(cv::vector<cv::Point> &lane_corners, int &detected_middle_pos_near, int &detected_middle_pos_far, int &left_lane_angle_st, int &right_lane_angle_st, int8_t &flags_to_UART);
*/
};
