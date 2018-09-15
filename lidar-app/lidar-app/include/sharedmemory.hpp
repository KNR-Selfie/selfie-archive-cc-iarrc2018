#pragma once

#include <iostream>
#include <string.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>

#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#include <include/def.hpp>

class SharedMemory
{
    int mem_id;
    int mem_size;
    key_t  key;
    uint32_t *shared_variable;
    uint32_t *end_of_variable;

    // Methods
public:
    SharedMemory(key_t  k = 50000, int m_size = 12000);
    bool init();
    void push_point_data(std::vector<cv::Point> points);
    void pull_points_data(cv::Mat &out, cv::Scalar color);
    void push_additional_data(cv::Point lidar_pos, int scale, cv::Point gap_pos_left, cv::Point gap_pos_right);
    void pull_additional_data();

    void close();
};
