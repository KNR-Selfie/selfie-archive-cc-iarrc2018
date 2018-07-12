#pragma once

#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#include <iostream>
#include <string.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>

class SharedMemory
{
    int mem_id;
    int mem_size;
    key_t  key;
    uint32_t *shared_variable;
    uint32_t *end_of_variable;

public:
    SharedMemory(key_t  k = 50000, int m_size = 100000);
    bool init();
    bool get_access();
    void push_data(std::vector<cv::Point> data);
    void pull_data(cv::Mat &frame);
    void close();
};

