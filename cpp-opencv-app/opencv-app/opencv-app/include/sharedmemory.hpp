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
    key_t  key;
    char *shared_variable;
    char *end_of_variable;

public:
    SharedMemory(key_t  k);
    bool init();
    bool get_access();
    void push_data(std::vector<std::vector<cv::Point>> vector);
    void pull_data();
    void close();
};

