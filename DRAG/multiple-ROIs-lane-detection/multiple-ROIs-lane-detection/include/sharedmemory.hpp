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
    SharedMemory(key_t  k = 50000, int m_size = 100000);
    bool init();
    void push_lane_data(std::vector<cv::Point> left_points, std::vector<cv::Point> right_points, std::vector<cv::Point> vector_cones);
    void pull_lane_data(cv::Mat &test);
    void push_scene_data(uint32_t red_light_visible, uint32_t green_light_visible);
    void pull_scene_data(uint32_t &reset_stop, uint32_t &reset_lane, uint32_t &light_3_pos, uint32_t &ping);

    void close();
};
