#pragma once

#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"



class LaneDetector
{
public:
    LaneDetector();
    void applyBlur(cv::Mat &input, cv::Mat &output);
    static void on_yellow_H_trackbar(int, void*);
    static void on_yellow_S_trackbar(int, void*);
    static void on_yellow_V_trackbar(int, void*);
    static void on_thresh_trackbar(int, void*);
    static void on_accuracy_trackbar(int, void*);
    void CreateTrackbars();
    void Hsv(cv::Mat frame_in, cv::Mat &yellow_frame_out, cv::Mat &white_frame_out, cv::Mat &yellow_canny, cv::Mat &white_canny);
    void BirdEye(cv::Mat frame_in, cv::Mat &frame_out);
    void UndistXML(cv::Mat &cameraMatrix, cv::Mat &distCoeffs);
    void Undist(cv::Mat frame_in, cv::Mat &frame_out, cv::Mat cameraMatrix, cv::Mat distCoeffs);
    void colorTransform(cv::Mat &input, cv::Mat &output);
    void edgeDetect();
    void detectLine(cv::Mat &input, std::vector<std::vector<cv::Point>> &output);
    void drawData(cv::Mat &frame, std::vector<std::vector<cv::Point>> &data_points);
};
