#pragma once

#include <iostream>

#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#include "urg_sensor.h"
#include "urg_utils.h"

#define FRAME_X 1000
#define FRAME_Y 1000

class Lidar
{
    // Handle for lidar operations
    urg_t urg;

    // Serial communication parameters
    const char connect_device[13] = "/dev/ttyACM0";
    const long connect_baudrate = 115200;

    int data_size;
    long *data;
    int n;
    int angle_offset = 0;
    std::vector<cv::Point> data_points;

public:
    Lidar();
    int init();
    void allocate_memory();
    void calculate_offset();
    void start_measurement();
    void read_new_data();
    void polar_to_cartesian();
    void draw_data(cv::Mat &data_frame);
    void stop_measurement();
};
