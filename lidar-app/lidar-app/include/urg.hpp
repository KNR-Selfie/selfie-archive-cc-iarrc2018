#pragma once

#include <iostream>

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#include <urg_c/urg_sensor.h>
#include <urg_c/urg_utils.h>

#include <include/def.hpp>

class URG
{
    // Handler for lidar operations
    urg_t urg;

    int n;
    int data_size;
    int angle_offset;

public:
    int scale = 2;
    int cut_left = 0;
    int cut_right = 450;
    int closest = 90;
    int farthest = 2000;
    int x_left = 0;
    int x_right = WIDTH;
    int y_up = 0;
    int y_down = 875;
    int thresh = 10;

    LidarReading raw_data;
    LidarReading filtered_data[3];

public:
    URG();
    int init();
    int read();
    void polar_to_cartesian();
    void filter_angle(LidarReading &input, LidarReading &output);
    void filter_distance(LidarReading &input, LidarReading &output);
    void filter_X(LidarReading &input, LidarReading &output);
    void filter_Y(LidarReading &input, LidarReading &output);
    void filter_single_points(LidarReading &input, LidarReading &output);
    void draw_data_raw(cv::Mat &frame);
    void draw_data_filtered(cv::Mat &frame, LidarReading &filtered);
    void draw_boundaries(cv::Mat &frame, LidarReading &input);

    void close();
};
