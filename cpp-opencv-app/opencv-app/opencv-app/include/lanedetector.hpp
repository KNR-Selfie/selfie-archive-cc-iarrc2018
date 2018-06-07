#pragma once

#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"



class LaneDetector
{    
public:
    LaneDetector();
    void applyBlur(cv::Mat &input, cv::Mat &output);
    void UndistXML(cv::Mat &cameraMatrix, cv::Mat &distCoeffs);
    void Undist(cv::Mat frame_in, cv::Mat &frame_out, cv::Mat cameraMatrix, cv::Mat distCoeffs);
    void BirdEye(cv::Mat frame_in, cv::Mat &frame_out);
    void CreateTrackbars();
    static void on_yellow_H_trackbar(int, void*);
    static void on_white_H_trackbar(int, void*);
    static void on_yellow_S_trackbar(int, void*);
    static void on_white_S_trackbar(int, void*);
    static void on_yellow_V_trackbar(int, void*);
    static void on_white_V_trackbar(int, void*);
    static void on_yellow_thresh_trackbar(int, void*);
    static void on_white_thresh_trackbar(int, void*);
    static void on_accuracy_trackbar(int, void*);
    static void on_f_trackbar(int, void*);
    static void on_acc_trackbar(int,void*);
    void Hsv(cv::Mat frame_in, cv::Mat &yellow_frame_out, cv::Mat &white_frame_out, cv::Mat &yellow_canny, cv::Mat &white_canny);
    void colorTransform(cv::Mat &input, cv::Mat &output);
    void detectLine(cv::Mat &input, std::vector<std::vector<cv::Point>> &output);
    void drawPoints(std::vector<std::vector<cv::Point> > &input, cv::Mat &output);
};
