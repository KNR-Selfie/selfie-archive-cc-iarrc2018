#pragma once

//#define DEBUG_MODE
//#define VERBOSE_MODE
//#define SHM_TEST
#define FPS_COUNT

#define FRAME_TIME 1
#define FPS_AMOUNT 250

#define WIDTH 1600
#define HEIGHT 1800

#ifndef DEBUG_MODE
#undef VERBOSE_MODE
#endif

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

struct LidarReading
{
    long *data;
    std::vector<cv::Point> pos;
    std::vector<double> angle;
};

