#include "lanedetector.hpp"
#include <iostream>
#include <string>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#define PI 3.1415926

static const int H_slider_max = 360;
static const int S_slider_max = 255;
static const int V_slider_max = 255;
static const int Thresh_max = 255;
static const int Accuracy_max = 100;

static int H_yellow_slider = 191, S_yellow_slider = 139, V_yellow_slider = 46, yel_Thresh_sider = 102;
static int H_white_slider, S_white_slider, V_white_slider, whi_Thresh_sider;
static int A_slider = 50;
static int H_yellow = 191, V_yellow = 139, S_yellow = 46, Thresh_yellow = 102;
static int H_white, V_white, S_white, Thresh_white;
static int A_value = 50;

LaneDetector::LaneDetector()
{

}

void LaneDetector::applyBlur(cv::Mat &input, cv::Mat &output)
{
    cv::GaussianBlur(input, output, cv::Size(3,3), 0, 0);
}

void LaneDetector::UndistXML(cv::Mat &cameraMatrix, cv::Mat &distCoeffs)
{
    std::string filename = "/home/selfie/Desktop/out_camera_data.xml";
    std::cout << "Reading XML" << std::endl;
    cv::FileStorage fs;
    fs.open(filename, cv::FileStorage::READ);

    if (!fs.isOpened())
    {
        std::cerr << "Failed to open " << filename << std::endl;
    }

    fs["Camera_Matrix"] >> cameraMatrix;
    fs["Distortion_Coefficients"] >> distCoeffs;
}

void LaneDetector::Undist(cv::Mat frame_in, cv::Mat &frame_out, cv::Mat cameraMatrix, cv::Mat distCoeffs)
{
    cv::undistort(frame_in, frame_out, cameraMatrix, distCoeffs);
}

void LaneDetector::BirdEye(cv::Mat frame_in, cv::Mat &frame_out)
{
    int alpha_ = 5;
	int f_ = 500, dist_ = 500;

	double f, dist;
	double alpha;
	alpha = ((double)alpha_ - 90.)*PI / 180;
	f = (double)f_;
	dist = (double)dist_;

    cv::Size taille = frame_in.size();
	double w = (double)taille.width, h = (double)taille.height;

    cv::Mat A1 = (cv::Mat_<float>(4, 3) <<
		1, 0, -w / 2,
		0, 1, -h / 2,
		0, 0, 0,
		0, 0, 1);

    cv::Mat RX = (cv::Mat_<float>(4, 4) <<
		1, 0, 0, 0,
		0, cos(alpha), -sin(alpha), 0,
		0, sin(alpha), cos(alpha), 0,
		0, 0, 0, 1);

    cv::Mat R = RX;

    cv::Mat T = (cv::Mat_<float>(4, 4) <<
		1, 0, 0, 0,
		0, 1, 0, 0,
		0, 0, 1, dist,
		0, 0, 0, 1);

    cv::Mat A2 = (cv::Mat_<float>(3, 4) <<
		f, 0, w / 2, 0,
		0, f, h / 2, 0,
		0, 0, 1, 0
		);

    cv::Mat transfo = A2 * (T * (R * A1));

    cv::warpPerspective(frame_in, frame_out, transfo, taille, cv::INTER_CUBIC | cv::WARP_INVERSE_MAP);
}

void LaneDetector::CreateTrackbars()
{
    cv::namedWindow("Yellow Line", 1);
    //cv::namedWindow("White Line", 1);
    cv::namedWindow("BirdEye", 1);
    cv::createTrackbar("H", "Yellow Line", &H_yellow_slider, H_slider_max, LaneDetector::on_yellow_H_trackbar);
    cv::createTrackbar("S", "Yellow Line", &S_yellow_slider, S_slider_max, LaneDetector::on_yellow_S_trackbar);
    cv::createTrackbar("V", "Yellow Line", &V_yellow_slider, V_slider_max, LaneDetector::on_yellow_V_trackbar);
    cv::createTrackbar("T", "Yellow Line", &yel_Thresh_sider, Thresh_max, LaneDetector::on_thresh_trackbar);
    cv::createTrackbar("H", "White Line", &H_white_slider, H_slider_max, LaneDetector::on_yellow_H_trackbar);
    cv::createTrackbar("S", "White Line", &S_white_slider, S_slider_max, LaneDetector::on_yellow_S_trackbar);
    cv::createTrackbar("V", "White Line", &V_white_slider, V_slider_max, LaneDetector::on_yellow_V_trackbar);
    cv::createTrackbar("T", "White Line", &whi_Thresh_sider, Thresh_max, LaneDetector::on_thresh_trackbar);
    cv::createTrackbar("A", "BirdEye", &A_slider, Accuracy_max, LaneDetector::on_accuracy_trackbar);
}

void LaneDetector::on_yellow_H_trackbar(int, void*)
{
        H_yellow = H_yellow_slider;
        H_white = H_white_slider;
}
void LaneDetector::on_yellow_S_trackbar(int, void*)
{
        S_yellow = S_yellow_slider;
        S_white = S_white_slider;
}
void LaneDetector::on_yellow_V_trackbar(int, void*)
{
        V_yellow = V_yellow_slider;
        V_white = V_white_slider;
}
void LaneDetector::on_thresh_trackbar(int, void*)
{
        Thresh_yellow = yel_Thresh_sider;
        Thresh_white = whi_Thresh_sider;
}

void LaneDetector::on_accuracy_trackbar(int, void*)
{
        A_value = A_slider;
}

void LaneDetector::Hsv(cv::Mat frame_in, cv::Mat &yellow_frame_out, cv::Mat &white_frame_out, cv::Mat &yellow_canny, cv::Mat &white_canny)
{
        cv::Mat imageHSV, yel_CannyHSV, whi_CannyHSV;
        cv::Mat kernel_v;
        kernel_v = cv::Mat(1, 3, CV_32F);
        kernel_v.at<float>(0, 0) = -1;
        kernel_v.at<float>(0, 1) = 0;
        kernel_v.at<float>(0, 2) = 1;

        cv::Point anchor;
        anchor = cv::Point(-1, -1);

        cv::Mat yel_Line_frame;
        cv::Mat whi_Line_frame;
        std::vector<std::vector<cv::Point> > contours;
        std::vector<cv::Vec4i> hierarchy;

        cvtColor(frame_in, frame_in, cv::COLOR_BGR2RGB);
        cvtColor(frame_in, imageHSV, cv::COLOR_BGR2HSV);
        cv::Vec3b yPixel(H_yellow, S_yellow, V_yellow);
        cv::Vec3b wPixel(H_white, S_white, V_white);

        cv::Mat3b yel(yPixel);
        cv::Mat3b whi(wPixel);

        cv::Mat3b yel_hsv;
        cv::Mat3b whi_hsv;
        cv::cvtColor(yel, yel_hsv, cv::COLOR_BGR2HSV);
        cv::cvtColor(whi, whi_hsv, cv::COLOR_BGR2HSV);
        cv::Vec3b yel_hsvPixel(yel_hsv.at<cv::Vec3b>(0, 0));
        cv::Vec3b whi_hsvPixel(whi_hsv.at<cv::Vec3b>(0, 0));

        int yel_thresh = Thresh_yellow;
        int whi_thresh = Thresh_white;

        cv::Scalar yel_minHSV = cv::Scalar(yel_hsvPixel.val[0] - yel_thresh, yel_hsvPixel.val[1] - yel_thresh, yel_hsvPixel.val[2] - yel_thresh);
        cv::Scalar yel_maxHSV = cv::Scalar(yel_hsvPixel.val[0] + yel_thresh, yel_hsvPixel.val[1] + yel_thresh, yel_hsvPixel.val[2] + yel_thresh);

        cv::Scalar whi_minHSV = cv::Scalar(whi_hsvPixel.val[0] - whi_thresh, whi_hsvPixel.val[1] - whi_thresh, whi_hsvPixel.val[2] - whi_thresh);
        cv::Scalar whi_maxHSV = cv::Scalar(whi_hsvPixel.val[0] + whi_thresh, whi_hsvPixel.val[1] + whi_thresh, whi_hsvPixel.val[2] + whi_thresh);

        cv::Mat yel_maskHSV, yel_resultHSV;
        cv::Mat whi_maskHSV, whi_resultHSV;
        inRange(frame_in, yel_minHSV, yel_maxHSV, yel_maskHSV);
        inRange(frame_in, whi_minHSV, whi_maxHSV, whi_maskHSV);
        bitwise_and(imageHSV, imageHSV, yel_resultHSV, yel_maskHSV);
        bitwise_and(imageHSV, imageHSV, whi_resultHSV, whi_maskHSV);

        //erode(yel_resultHSV, yel_resultHSV, cv::Mat(), cv::Point(-1, -1), 3);
        //dilate(yel_resultHSV, yel_resultHSV, cv::Mat(), cv::Point(-1, -1), 3);
        erode(whi_resultHSV, whi_resultHSV, cv::Mat(), cv::Point(-1, -1), 3);
        dilate(whi_resultHSV, whi_resultHSV, cv::Mat(), cv::Point(-1, -1), 3);

        medianBlur(yel_resultHSV, yel_resultHSV, 5);
        medianBlur(whi_resultHSV, whi_resultHSV, 5);
        //threshold(yel_resultHSV, yel_resultHSV, 30, 255, cv::THRESH_BINARY);
        //threshold(whi_resultHSV, whi_resultHSV, 30, 255, cv::THRESH_BINARY);
        filter2D(yel_resultHSV, yel_Line_frame, -1, kernel_v, anchor, 0, cv::BORDER_DEFAULT);
        filter2D(whi_resultHSV, whi_Line_frame, -1, kernel_v, anchor, 0, cv::BORDER_DEFAULT);

        Canny(yel_resultHSV, yel_CannyHSV, 150, 300, 3);
        Canny(whi_resultHSV, whi_CannyHSV, 150, 300, 3);

        cv::findContours(yel_CannyHSV, contours, hierarchy, CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE, cv::Point(0, 0));

        yellow_frame_out = yel_resultHSV;
        yellow_canny = yel_Line_frame;

        white_frame_out = whi_resultHSV;
        white_canny = whi_Line_frame;
}

void LaneDetector::colorTransform(cv::Mat &input, cv::Mat &output)
{
    cv::cvtColor(input, input, cv::COLOR_RGB2GRAY);
    cv::threshold(input, output, 1, 255, 1);
}



void LaneDetector::detectLine(cv::Mat &input, std::vector<std::vector<cv::Point>> &output)
{
    std::vector<cv::Vec4i> hierarchy;
    int accuracy = 5;

    cv::findContours(input, output, hierarchy, CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE, cv::Point(0, 0));

    for (int i = 0; i < output.size(); i++)
    {
        cv::approxPolyDP(cv::Mat(output[i]),output[i], accuracy, false);
    }
}

void LaneDetector::drawPoints(std::vector<std::vector<cv::Point>> &input, cv::Mat &output)
{
    for (int i = 0; i < input.size(); i++)
    {
        for (unsigned int j = 0; j < input[i].size(); j++)
        {
            cv::circle(output, input[i][j], 3, cv::Scalar(0, 255, 255), CV_FILLED, cv::LINE_AA);

            if (j>0)
                cv::line(output, cv::Point(input[i][j - 1]), cv::Point(input[i][j]), cv::Scalar(0, 0, 255), 2);
        }
    }
}
