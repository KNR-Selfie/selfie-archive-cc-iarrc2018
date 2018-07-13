#pragma once

#include <iostream>

#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/objdetect/objdetect.hpp>

#include <include/def.hpp>

class DetectionArea
{
private:
    cv::Rect def_roi;
public:
    cv::Rect roi;
    cv::Mat frame_roi;
    cv::Mat frame_reduced;
    cv::Point middle_roi;
    cv::Point middle_detection;
    cv::Point left, right;
    uchar *p;
    int roi_x = ROI_X_SEARCH;
    bool detected = false;

public:
    DetectionArea();
    void init(cv::Point center);
    void reset();
    void update(cv::Mat &input);
    void find_center(int thresh);
    void check_boundaries();
};

class LaneDetector
{
    // Birdeye
public:
    int f_i = 200;
    int alpha_i = 64;

    int dist_i = 40;
    int dist_inv_i = 260;
    int cut_y = 0;

private:
    double f = (double)f_i;    
    double alpha = ((double)alpha_i - 90.)*CV_PI / 180;

    double dist = (double)dist_i;
    double dist_inv = (double)dist_inv_i;

    cv::Size taille;
    double w, h;

    cv::Size taille_inv;
    double w_inv, h_inv;

    cv::Mat A1;
    cv::Mat A2;
    cv::Mat RX;
    cv::Mat R;
    cv::Mat T;
    cv::Mat K;

    cv::Mat A1_inv;
    cv::Mat T_inv;
    cv::Mat K_inv;

    cv::Rect tmp_rect = cv::Rect(0, 0, CAM_RES_X, CAM_RES_Y - cut_y);

    cv::Mat transfo;
    cv::Mat transfo_inv;

    // Opening, Closing
    int erosion_size = 1;
    int dilation_size = 3;

    cv::Mat element_erosion = getStructuringElement(cv::MORPH_RECT,
                    cv::Size(2 * erosion_size + 1, 2 * erosion_size + 1),
                    cv::Point(erosion_size, erosion_size) );

    cv::Mat element_dilation = getStructuringElement(cv::MORPH_RECT,
                    cv::Size(2 * dilation_size + 1, 2 * dilation_size + 1),
                    cv::Point(dilation_size, dilation_size) );

    // Stop
    cv::Rect roi_stop;
    cv::Mat frame_stop;
    cv::Mat frame_stop_reduced;
    cv::Point stop_middle;
    cv::Point stop_left, stop_right;
    uchar *stop_p;

public:
    bool stop_detected = false;
    uint32_t stop_distance = 0;
    bool display_stop_roi = true;

    //Thresh calculation
private:
    cv::Rect tmp[DETECTION_NUMBER];
    cv::Mat roi_thresh;

    // Methods
public:
    LaneDetector();
    void calculate_bird_var(cv::Mat &frame_ref, cv::Mat &frame_inv_ref);
    void bird_eye(cv::Mat &input, cv::Mat &output);
    void bird_eye_inverse(cv::Mat &input, cv::Mat &output);
    void binarize(cv::Mat &input, cv::Mat &output, int &thresh_low, int &thresh_high);
    void opening(cv::Mat &input, cv::Mat &output);
    void closing(cv::Mat &input, cv::Mat &output);
    void apply_sobel(cv::Mat &input, cv::Mat &output);
    void correct_ROIs(DetectionArea input[], cv::Mat &frame);
    void calculate_thresh(cv::Mat &frame, DetectionArea area_line[]);
    void search_stop_line(DetectionArea area_line[], cv::Mat &input, int thresh_stop);
    void draw_data(cv::Mat &input, DetectionArea area_line[]);
    void pack_data(DetectionArea area_left_line[], std::vector<cv::Point> &left_points);
};
