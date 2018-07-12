#pragma once

#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#include <iostream>
#include <string.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>

#define MEMSIZE 16384
/*
uint32_t velocity = 500;
uint32_t servo_angle = 90000;
unsigned char reset_STM = 0;
unsigned char red_light_detected = 0;
unsigned char green_light_detected = 0;
unsigned char stop_line_detected = 0;
uint32_t stop_distance = 0;
*/

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
    void push_data(std::vector<std::vector<cv::Point>> vector);
    void pull_line_data(std::vector<cv::Point> &y_vector,std::vector<cv::Point> &w_vector,std::vector<cv::Point> &c_vector);
    void pull_lidar_data(std::vector<cv::Point>&l_vector);
    void pull_usb_data(std::vector<uint32_t>&data);
    void push_signal(uint32_t signal);
    bool pull_signal();

    void close();
    uint32_t get_lenght();
};

