#include "lanedetector.hpp"

//#define LANETIMING
#ifdef LANETIMING
#include <chrono>
#define INIT_LANETIMER static auto startlane = std::chrono::high_resolution_clock::now();
#define START_LANETIMER  startlane = std::chrono::high_resolution_clock::now();
#define STOP_LANETIMER(name)  std::cout << "RUNTIME of " << name << ": " << \
    std::chrono::duration_cast<std::chrono::microseconds>( \
            std::chrono::high_resolution_clock::now()-startlane \
    ).count() << " us " << std::endl;
#else
#define INIT_LANETIMER
#define START_LANETIMER
#define STOP_LANETIMER(name)
#endif

static const int H_slider_max = 360;
static const int S_slider_max = 255;
static const int V_slider_max = 255;
static const int Thresh_max = 255;
static const int Accuracy_max = 100;
static const int F_max = 1000;

static int A_slider = 13;
static int K_slider = 973;
static int H_yellow = 110, V_yellow = 49, S_yellow = 102, Thresh_yellow = 50;
static int H_white = 123, V_white =102, S_white = 60, Thresh_white = 33;
static int A_value = 50;
static int K_value = 50;
static int Acc_slider = 1;
static int Acc_value = 1;
static int Acc_filt = 15;
static int Acc_filt_slider = 40;

static int R_down = 89, G_down = 0, B_down = 0;
static int R_up = 112, G_up = 255, B_up = 255;

static int H_down = 20, S_down = 40, V_down = 90;
static int H_up = 40, S_up = 255, V_up = 255;

static int H_down_white = 48, S_down_white = 104, V_down_white = 94;
static int H_up_white = 143, S_up_white = 207, V_up_white = 183;

cv::Mat tmp_mat(IDS_HEIGHT, IDS_WIDTH, CV_8UC3);

LaneDetector::LaneDetector()
{
}

void LaneDetector::applyBlur(cv::Mat &input, cv::Mat &output)
{
    cv::GaussianBlur(input, output, cv::Size(3,3), 0, 0);
}

void LaneDetector::UndistXML(cv::Mat &cameraMatrix, cv::Mat &distCoeffs)
{
    std::string filename = "./../opencv-app/out_camera_data.xml";
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

void LaneDetector::calculate_bird_var(cv::Mat frame_ref, cv::Mat frame_inv_ref)
{
    alpha = ((double)alpha_i - 90.)*CV_PI / 180;
    dist = (double)dist_i;
    dist_inv = (double)dist_inv_i;
    f = (double)f_i;

    taille = frame_ref.size();
    w = (double)taille.width, h = (double)taille.height;

    taille_inv = frame_inv_ref.size();
    w_inv = (double)taille_inv.width, h_inv = (double)taille_inv.height;

    A1 = (cv::Mat_<float>(4, 3) <<
        1, 0, -w / 2,
        0, 1, -h / 2,
        0, 0, 0,
        0, 0, 1);

    RX = (cv::Mat_<float>(4, 4) <<
        1, 0, 0, 0,
        0, cos(alpha), -sin(alpha), 0,
        0, sin(alpha), cos(alpha), 0,
        0, 0, 0, 1);

    R = RX;

    T = (cv::Mat_<float>(4, 4) <<
        1, 0, 0, 0,
        0, 1, 0, 0,
        0, 0, 1, dist,
        0, 0, 0, 1);

    A2 = (cv::Mat_<float>(3, 4) <<
        f, 0, w / 2, 0,
        0, f, h / 2, 0,
        0, 0, 1, 0
        );

    K = (cv::Mat_<float>(3, 4) <<
        f, 0, w / 2, 0,
        0, f, h / 2, 0,
        0, 0, 1, 0
        );

    A1_inv = (cv::Mat_<float>(4, 3) <<
        1, 0, -w_inv / 2,
        0, 1, -h_inv / 2,
        0, 0, 0,
        0, 0, 1);

    T_inv = (cv::Mat_<float>(4, 4) <<
        1, 0, 0, 0,
        0, 1, 0, 0,
        0, 0, 1, dist_inv,
        0, 0, 0, 1);

    K_inv = (cv::Mat_<float>(3, 4) <<
        f, 0, w_inv / 2, 0,
        0, f, h_inv / 2, 0,
        0, 0, 1, 0
        );    void pack_points();    void pack_points();

    tmp_rect = cv::Rect(0, 0, IDS_WIDTH, IDS_HEIGHT - cut_y);

    transfo = K * (T * (R * A1));
    transfo_inv = K_inv * (T_inv * (R * A1_inv));
}

void LaneDetector::bird_eye(cv::Mat &input, cv::Mat &output)
{
    cv::warpPerspective(input, tmp_mat, transfo, taille, cv::INTER_CUBIC | cv::WARP_INVERSE_MAP);
    tmp_mat(tmp_rect).copyTo(output);
}

void LaneDetector::CreateTrackbars()
{
    cv::namedWindow("1.1 Yellow Line", 1);
    cv::namedWindow("1.2 White Line", 1);
//    cv::namedWindow("3.1 Yellow Bird Eye", 1);
//    cv::namedWindow("3.2 White Bird Eye", 1);
//    cv::namedWindow("3.3 Line Accuracy",1);
//    cv::namedWindow("5 Cone Detect", 1);
    cv::namedWindow("4.1 Yellow Vector", 1);
    cv::createTrackbar("Acc", "4.1 Yellow Vector", &Acc_value, 255, NULL);
    cv::createTrackbar("Acc_filt", "4.1 Yellow Vector", &Acc_filt, 50, NULL);
//    cv::createTrackbar("Rdown", "5 Cone Detect", &R_down, 255, NULL);
//    cv::createTrackbar("Rup", "5 Cone Detect", &R_up, 255, NULL);
//    cv::createTrackbar("Gdown", "5 Cone Detect", &G_down, 255, NULL);
//    cv::createTrackbar("Gup", "5 Cone Detect", &G_up, 255, NULL);
//    cv::createTrackbar("Bdown", "5 Cone Detect", &B_down, 255, NULL);
//    cv::createTrackbar("Bup", "5 Cone Detect", &B_up, 255, NULL);
    cv::createTrackbar("Hdown", "1.1 Yellow Line", &H_down, 255, NULL);
    cv::createTrackbar("Hup", "1.1 Yellow Line", &H_up, 255, NULL);
    cv::createTrackbar("Sdown", "1.1 Yellow Line", &S_down, 255, NULL);
    cv::createTrackbar("Sup", "1.1 Yellow Line", &S_up, 255, NULL);
    cv::createTrackbar("Vdown", "1.1 Yellow Line", &V_down, 255, NULL);
    cv::createTrackbar("Vup", "1.1 Yellow Line", &V_up, 255, NULL);

    cv::createTrackbar("Hdown", "1.2 White Line", &H_down_white, 255, NULL);
    cv::createTrackbar("Hup", "1.2 White Line", &H_up_white, 255, NULL);
    cv::createTrackbar("Sdown", "1.2 White Line", &S_down_white, 255, NULL);
    cv::createTrackbar("Sup", "1.2 White Line", &S_up_white, 255, NULL);
    cv::createTrackbar("Vdown", "1.2 White Line", &V_down_white, 255, NULL);
    cv::createTrackbar("Vup", "1.2 White Line", &V_up_white, 255, NULL);
}

void LaneDetector::Hsv(cv::Mat frame_in, cv::Mat &yellow_frame_out, cv::Mat &white_frame_out, cv::Mat &yellow_canny, cv::Mat &white_canny)
{
        kernel_v = cv::Mat(1, 3, CV_32F);
        kernel_v.at<float>(0, 0) = -1;
        kernel_v.at<float>(0, 1) = 0;
        kernel_v.at<float>(0, 2) = 1;


        cvtColor(frame_in, frame_in, cv::COLOR_BGR2RGB);
        cvtColor(frame_in, imageHSV, cv::COLOR_RGB2HSV);

        cv::Vec3b yPixel(H_yellow, S_yellow, V_yellow);
        cv::Vec3b wPixel(H_white, S_white, V_white);

        cv::Mat3b yel(yPixel);
        cv::Mat3b whi(wPixel);

        cv::cvtColor(yel, yel_hsv, cv::COLOR_BGR2HSV);
        cv::cvtColor(whi, whi_hsv, cv::COLOR_BGR2HSV);

        cv::Vec3b yel_hsvPixel(yel_hsv.at<cv::Vec3b>(0, 0));
        cv::Vec3b whi_hsvPixel(whi_hsv.at<cv::Vec3b>(0, 0));

        yel_thresh = Thresh_yellow;
        whi_thresh = Thresh_white;

        cv::Scalar yel_minHSV = cv::Scalar(H_down, S_down, V_down);
        cv::Scalar yel_maxHSV = cv::Scalar(H_up, S_up, V_up);

        cv::Scalar whi_minHSV = cv::Scalar(H_down_white, S_down_white, V_down_white);
        cv::Scalar whi_maxHSV = cv::Scalar(H_up_white, S_up_white, V_up_white);

        cv::Mat yel_maskHSV, yel_resultHSV;
        cv::Mat whi_maskHSV, whi_resultHSV;

        inRange(frame_in, yel_minHSV, yel_maxHSV, yel_maskHSV);
        inRange(frame_in, whi_minHSV, whi_maxHSV, whi_maskHSV);
        bitwise_and(imageHSV, imageHSV, yel_resultHSV, yel_maskHSV);
        bitwise_and(imageHSV, imageHSV, whi_resultHSV, whi_maskHSV);

        medianBlur(yel_resultHSV, yel_resultHSV, 5);
        medianBlur(whi_resultHSV, whi_resultHSV, 5);
        filter2D(yel_resultHSV, yel_Line_frame, -1, kernel_v, anchor, 0, cv::BORDER_DEFAULT);
        filter2D(whi_resultHSV, whi_Line_frame, -1, kernel_v, anchor, 0, cv::BORDER_DEFAULT);

        yellow_frame_out = yel_resultHSV;
        yellow_canny = yel_Line_frame;

        white_frame_out = whi_resultHSV;
        white_canny = whi_Line_frame;
}

void LaneDetector::Hsv_both(cv::Mat &frame_in, cv::Mat &yellow_frame_out, cv::Mat &white_frame_out, cv::Mat &yellow_canny, cv::Mat &white_canny)
{
        kernel_v = cv::Mat(1, 3, CV_32F);
        kernel_v.at<float>(0, 0) = -1;
        kernel_v.at<float>(0, 1) = 0;
        kernel_v.at<float>(0, 2) = 1;

        inRange(frame_in,cv::Scalar(H_down_white, S_down_white, V_down_white),cv::Scalar(H_up_white, S_up_white, V_up_white),white_frame_out);
        inRange(frame_in,cv::Scalar(H_down, S_down, V_down),cv::Scalar(H_up, S_up, V_up),yellow_frame_out);

        medianBlur(white_frame_out, white_frame_out, 5);
        medianBlur(yellow_frame_out, yellow_frame_out, 5);
        filter2D(white_frame_out, white_canny, -1, kernel_v, anchor, 0, cv::BORDER_DEFAULT);
        filter2D(yellow_frame_out,yellow_canny, -1, kernel_v, anchor, 0, cv::BORDER_DEFAULT);
}

void LaneDetector::detectLine(cv::Mat &input, std::vector<std::vector<cv::Point>> &output)
{
    output.clear();
    hierarchy_detectline.clear();
    cv::findContours(input, output, hierarchy_detectline, CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE, cv::Point(0, 0));
    for (std::vector<std::vector<cv::Point>>::iterator filt =output.begin();filt !=output.end();){
        if (filt->size()<Acc_filt)
            filt = output.erase(filt);
        else
            ++filt;
    }
    for (int i = 0; i < output.size(); i++)
    {
        cv::approxPolyDP(cv::Mat(output[i]),output[i], Acc_value, false);
    }
}

void LaneDetector::detectLine_both(cv::Mat &input_white, std::vector<std::vector<cv::Point>> &output_white,cv::Mat &input_yellow, std::vector<std::vector<cv::Point>> &output_yellow)
{
    output_white.clear();
    output_yellow.clear();
    hierarchy_detectline.clear();
    cv::findContours(input_white, output_white, hierarchy_detectline, CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE, cv::Point(0, 0));
    hierarchy_detectline.clear();
    cv::findContours(input_yellow, output_yellow, hierarchy_detectline, CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE, cv::Point(0, 0));

    for (std::vector<std::vector<cv::Point>>::iterator filt =output_white.begin();filt !=output_white.end();){
        if (filt->size()<Acc_filt)
            filt = output_white.erase(filt);
        else
            ++filt;
    }
    for (std::vector<std::vector<cv::Point>>::iterator filt =output_yellow.begin();filt !=output_yellow.end();){
        if (filt->size()<Acc_filt)
            filt = output_yellow.erase(filt);
        else
            ++filt;
    }

    for (int i = 0; i < output_white.size(); i++)
    {
        cv::approxPolyDP(cv::Mat(output_white[i]),output_white[i], Acc_value, false);
    }
    for (int i = 0; i < output_yellow.size(); i++)
    {
        cv::approxPolyDP(cv::Mat(output_yellow[i]),output_yellow[i], Acc_value, false);
    }
}

void LaneDetector::drawPoints(std::vector<std::vector<cv::Point>> &input, cv::Mat &output)
{
    cv::Mat frame_draw = cv::Mat(output.size(), CV_8UC3);
    for (int i = 0; i < input.size(); i++)
    {
        for (unsigned int j = 0; j < input[i].size(); j++)
        {
            cv::circle(frame_draw, input[i][j], 3, cv::Scalar(0, 255, 255), CV_FILLED, cv::LINE_AA);

            if (j>0)
                cv::line(frame_draw, cv::Point(input[i][j - 1]), cv::Point(input[i][j]), cv::Scalar(0, 0, 255), 2);
        }
    }
    frame_draw.copyTo(output);
}

void LaneDetector::drawPoints_both(std::vector<std::vector<cv::Point>> &input_white, cv::Mat &output_white,std::vector<std::vector<cv::Point>> &input_yellow, cv::Mat &output_yellow)
{
    output_white = cv::Mat::zeros(output_white.size(), CV_8UC3);
    output_yellow = cv::Mat::zeros(output_yellow.size(), CV_8UC3);
    for (int i = 0; i < input_white.size(); i++)
    {
        for (unsigned int j = 0; j < input_white[i].size(); j++)
        {
            cv::circle(output_white, input_white[i][j], 3, cv::Scalar(0, 255, 255), CV_FILLED, cv::LINE_AA);

            if (j>0)
                cv::line(output_white, cv::Point(input_white[i][j - 1]), cv::Point(input_white[i][j]), cv::Scalar(0, 0, 255), 2);
        }
    }
    for (int i = 0; i < input_yellow.size(); i++)
    {
        for (unsigned int j = 0; j < input_yellow[i].size(); j++)
        {
            cv::circle(output_yellow, input_yellow[i][j], 3, cv::Scalar(0, 255, 255), CV_FILLED, cv::LINE_AA);

            if (j>0)
                cv::line(output_yellow, cv::Point(input_yellow[i][j - 1]), cv::Point(input_yellow[i][j]), cv::Scalar(0, 0, 255), 2);
        }
    }
}

void LaneDetector::ConeDetection(cv::Mat frame_in, cv::Mat &frame_out, std::vector<cv::Point> &conePoints)
{
    hierarchy.clear();
    contours.clear();
    cv::cvtColor(frame_in, frame_tmp, cv::COLOR_BGR2RGB);
    cv::cvtColor(frame_tmp, image_HSV, cv::COLOR_RGB2HSV);
    cv::inRange(image_HSV, cv::Scalar(R_down, G_down, B_down), cv::Scalar(R_up, G_up, B_up), img_thresh_low);
    cv::inRange(image_HSV, cv::Scalar(159, 135, 135), cv::Scalar(179, 255, 255), img_thresh_high);
    cv::bitwise_or(image_HSV, image_HSV, result, img_thresh_low);
    cv::morphologyEx(result, result, cv::MORPH_OPEN, (5, 5));
    cv::medianBlur(result, result, 5);
    cv::cvtColor(result, result, cv::COLOR_BGR2GRAY);
    cv::threshold(result, result, 30, 255, cv::THRESH_BINARY);
    cv::Canny(result, frame_out, 80, 160, 3);
    cv::findContours(result, contours, hierarchy, CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE, cv::Point(0, 0));

    drawing = cv::Mat::zeros(result.size(), CV_8UC3);

    std::vector<cv::Rect> boundRect(contours.size());

    for (int i = 0; i < contours.size(); i++)
    {
        cv::drawContours(drawing, contours, i, cv::Scalar(255, 0, 0), 2, 7, hierarchy, 0, cv::Point());
        boundRect[i] = cv::boundingRect(cv::Mat(contours[i]));
        cv::rectangle(drawing, boundRect[i].tl(), boundRect[i].br(), CV_RGB(200, 200, 0), 2, 8, 0);

        cv::putText(drawing, label_cone, boundRect[i].tl() + cv::Point(0, -5), CV_FONT_HERSHEY_DUPLEX, 0.4, CV_RGB(0, 255, 0), 1.0);
    }

    for(int i = 0; i < boundRect.size(); i++)
    {
        cv::Point cone = cv::Point(boundRect[i].x, boundRect[i].y);
        cone.x = boundRect[i].x + (boundRect[i].width / 2);
        cone.y = boundRect[i].y + boundRect[i].height;
        conePoints.push_back(cone);
        cv::circle(drawing, cone, 1, CV_RGB(255, 255, 0), 6, 8, 0);
    }

    cv::Point fill[1][4];
    const cv::Point *ppt[1] = {fill[0]};
    int npt[] = {4};

    poly = cv::Mat::zeros(drawing.rows, drawing.cols, CV_8UC3);
    cv::fillPoly(poly, ppt, npt, 1, CV_RGB(0, 0 , 255), 4);

    cv::addWeighted(drawing, 0.7, poly, 0.3, 0.0, dst);
    dst.copyTo(frame_out);
}

void LaneDetector::ConeDetection_new(cv::Mat frame_in, cv::Mat &frame_out, std::vector<cv::Point> &conePoints)
{
    hierarchy.clear();
    contours.clear();
    cv::cvtColor(frame_in, image_HSV, cv::COLOR_BGR2HSV);
    cv::inRange(image_HSV, cv::Scalar(159, 135, 135), cv::Scalar(179, 255, 255), img_thresh_high);
    cv::bitwise_or(image_HSV, image_HSV, result, img_thresh_low);
    cv::morphologyEx(result, result, cv::MORPH_OPEN, (5, 5));
    cv::medianBlur(result, result, 5);
    cv::cvtColor(result, result, cv::COLOR_BGR2GRAY);
    cv::threshold(result, result, 30, 255, cv::THRESH_BINARY);
    cv::Canny(result, frame_out, 80, 160, 3);
    cv::findContours(result, contours, hierarchy, CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE, cv::Point(0, 0));

    drawing = cv::Mat::zeros(result.size(), CV_8UC3);

    std::vector<cv::Rect> boundRect(contours.size());

    for(int i = 0; i < boundRect.size(); i++)
    {
        cv::Point cone = cv::Point(boundRect[i].x, boundRect[i].y);
        cone.x = boundRect[i].x + (boundRect[i].width / 2);
        cone.y = boundRect[i].y + boundRect[i].height;
        conePoints.push_back(cone);
        cv::circle(drawing, cone, 1, CV_RGB(255, 255, 0), 6, 8, 0);
    }

    cv::Point fill[1][4];
    const cv::Point *ppt[1] = {fill[0]};
    int npt[] = {4};

    poly = cv::Mat::zeros(drawing.rows, drawing.cols, CV_8UC3);
    cv::fillPoly(poly, ppt, npt, 1, CV_RGB(0, 0 , 255), 4);

    cv::addWeighted(drawing, 0.7, poly, 0.3, 0.0, dst);
    dst.copyTo(frame_out);
}

void LaneDetector::StopLine(cv::Mat frame_in, cv::Mat &hist_frame, cv::Mat &hsv_frame, bool &flag)
{
    INIT_LANETIMER
    START_LANETIMER
    cv::Mat imageHSV;

    cv::Mat mag_Line_frame;
    std::vector<std::vector<cv::Point> > contours;
    std::vector<cv::Vec4i> hierarchy;

    cvtColor(frame_in, imageHSV, cv::COLOR_BGR2HSV);
    hsv_frame = imageHSV;

    cv::Scalar mag_minHSV = cv::Scalar(50, 0, 0);
    cv::Scalar mag_maxHSV = cv::Scalar(80, 100, 255);

    cv::Mat mag_maskHSV, mag_resultHSV;
    STOP_LANETIMER("Zmienne")
    START_LANETIMER
    inRange(frame_in, mag_minHSV, mag_maxHSV, mag_maskHSV);
    STOP_LANETIMER("inrange")
    START_LANETIMER
    bitwise_and(imageHSV, imageHSV, mag_resultHSV, mag_maskHSV);
    STOP_LANETIMER("bitwise")
    START_LANETIMER
    medianBlur(mag_resultHSV, mag_resultHSV, 5);
    STOP_LANETIMER("blur")
    START_LANETIMER

    cv::Mat mask = cv::Mat::zeros(cv::Size(mag_resultHSV.cols, mag_resultHSV.rows), CV_8UC1);
    cv::Point points[4] =
    {
        cv::Point(280, 0),
        cv::Point(mag_resultHSV.cols - 280, 0),
        cv::Point(mag_resultHSV.cols - 280, mag_resultHSV.rows),
        cv::Point(280, mag_resultHSV.rows)
    };

    cv::fillConvexPoly(mask, points, 4, cv::Scalar(255, 0, 0));
    cv::Mat mask_out;
    cv::bitwise_and(mag_resultHSV, mag_resultHSV, mask_out, mask);

    hsv_frame = mask_out;
    STOP_LANETIMER("maska")
    START_LANETIMER

    std::vector<cv::Mat> bgr_planes;
    split(mask_out, bgr_planes);
    STOP_LANETIMER("split")
    START_LANETIMER
    int histSize = 256;

    float range[] = { 0, 256 };
    const float* histRange = { range };

    bool uniform = true; bool accumulate = false;

    cv::Mat b_hist, g_hist, r_hist;

    cv::calcHist(&bgr_planes[0], 1, 0, cv::Mat(), b_hist, 1, &histSize, &histRange, uniform, accumulate);

    int hist_w = 512; int hist_h = 400;
    int bin_w = cvRound((double)hist_w / histSize);

    cv::Mat histImage(hist_h, hist_w, CV_8UC3, cv::Scalar(0, 0, 0));

    cv::normalize(b_hist, b_hist, 0, histImage.rows, cv::NORM_MINMAX, -1, cv::Mat());

    std::vector<cv::Point> b_p_vec;

    for (int i = 1; i < histSize; i++)
    {
        cv::line(histImage, cv::Point(bin_w*(i - 1), hist_h - cvRound(b_hist.at<float>(i - 1))),
            cv::Point(bin_w*(i), hist_h - cvRound(b_hist.at<float>(i))),
            cv::Scalar(255, 0, 0), 2, 8, 0);
        b_p_vec.push_back(cv::Point(bin_w*(i - 1), hist_h - cvRound(b_hist.at<float>(i - 1))));
    }
    hist_frame = histImage;
    STOP_LANETIMER("histogram")
    START_LANETIMER
    int tmp_maxh = hist_h;
    int tmp_imaxh = 0;
    for (int i = 0; i < histSize; i++)
    {
        if (hist_h - cvRound(b_hist.at<float>(i)) < tmp_maxh && hist_h - cvRound(b_hist.at<float>(i)) != 0)
        {
            tmp_maxh = hist_h - cvRound(b_hist.at<float>(i));
            tmp_imaxh = i;
        }
    }

    if (tmp_imaxh > 150 && tmp_imaxh < 175)
    {
        flag = true;
//        std::cout << "STOP" << std::endl;
    }
    else
    {
        flag = false;
    }
    STOP_LANETIMER("reszta")
}

 void LaneDetector::StopLine_v2(cv::Mat frame_in, cv::Mat &hsv_frame, bool &flag)
 {
     INIT_LANETIMER
     START_LANETIMER
     cv::Mat imageHSV;
     cv::Scalar white_pix;

     cv::Scalar mag_minHSV = cv::Scalar(50, 0, 0);
     cv::Scalar mag_maxHSV = cv::Scalar(80, 100, 255);

     cv::Mat mag_maskHSV, mag_resultHSV;
     STOP_LANETIMER("Zmienne")
     START_LANETIMER
     //cvtColor(frame_in, imageHSV, cv::COLOR_BGR2HSV);
     STOP_LANETIMER("cvtcolor")
     START_LANETIMER
     inRange(frame_in, mag_minHSV, mag_maxHSV, mag_maskHSV);
     STOP_LANETIMER("inrange")
     START_LANETIMER
     bitwise_and(frame_in, frame_in, mag_resultHSV, mag_maskHSV);
     STOP_LANETIMER("bitwise&")
     START_LANETIMER
     medianBlur(mag_resultHSV, mag_resultHSV, 5);
     STOP_LANETIMER("medianblur")
     START_LANETIMER

     cv::Mat mask = cv::Mat::zeros(cv::Size(mag_resultHSV.cols, mag_resultHSV.rows), CV_8UC1);
     cv::Point points[4] =
     {
         cv::Point(280, 0),
         cv::Point(mag_resultHSV.cols - 280, 0),
         cv::Point(mag_resultHSV.cols - 280, mag_resultHSV.rows),
         cv::Point(280, mag_resultHSV.rows)
     };
     cv::fillConvexPoly(mask, points, 4, cv::Scalar(255, 0, 0));
     cv::Mat mask_out;
     cv::bitwise_and(mag_maskHSV, mag_maskHSV, mask_out, mask);

     white_pix = cv::mean(mask_out);

     if(white_pix[0] > 1.5)
     {
         flag = true;
     }
     hsv_frame = mask_out;
     STOP_LANETIMER("maska")
     START_LANETIMER
 }
