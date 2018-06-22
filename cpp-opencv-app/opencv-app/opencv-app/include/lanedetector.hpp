#pragma once

#include <iostream>
#include <string>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#define PI 3.1415926

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
    static void on_acc_filt_trackbar(int,void*);
    //
    static void on_Rdown_trackbar(int,void*);
    static void on_Rup_trackbar(int,void*);
    static void on_Gdown_trackbar(int,void*);
    static void on_Gup_trackbar(int,void*);
    static void on_Bdown_trackbar(int,void*);
    static void on_Bup_trackbar(int,void*);
    //
    static void on_Hdown_trackbar(int,void*);
    static void on_Hup_trackbar(int,void*);
    static void on_Sdown_trackbar(int,void*);
    static void on_Sup_trackbar(int,void*);
    static void on_Vdown_trackbar(int,void*);
    static void on_Vup_trackbar(int,void*);
    //
    static void on_Hdown_trackbar_white(int,void*);
    static void on_Hup_trackbar_white(int,void*);
    static void on_Sdown_trackbar_white(int,void*);
    static void on_Sup_trackbar_white(int,void*);
    static void on_Vdown_trackbar_white(int,void*);
    static void on_Vup_trackbar_white(int,void*);


    void Hsv(cv::Mat frame_in, cv::Mat &yellow_frame_out, cv::Mat &white_frame_out, cv::Mat &yellow_canny, cv::Mat &white_canny);
    void colorTransform(cv::Mat &input, cv::Mat &output);
    void detectLine(cv::Mat &input, std::vector<std::vector<cv::Point>> &output);
    void drawPoints(std::vector<std::vector<cv::Point> > &input, cv::Mat &output);
    void ConeDetection(cv::Mat frame_in, cv::Mat &frame_out, std::vector<cv::Point> &conePoints);
};
