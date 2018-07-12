#pragma once

#include <iostream>
#include <string>

#include <opencv2/highgui/highgui.hpp>

#include <include/def.hpp>

class Kurokesu
{
public:
    cv::VideoCapture camera;

    int cam_brightness = 0;
    int cam_contrast = 32;
    int cam_saturation = 64;
    int cam_hue = 0;
    int cam_white_bal_auto = 1;
    int cam_gamma = 100;
    int cam_gain = 0;
    int cam_pwr_line_freq = 2;
    int cam_white_bal_temp = 4600;
    int cam_sharpness = 3;
    int cam_backlight_compensation = 1;
    int cam_exposure_auto = 3;
    int cam_exposure_absolute = 156;
    int cam_auto_priority = 1;

public:
    Kurokesu();
    bool init();
    void trackbars();
    void update();
};
