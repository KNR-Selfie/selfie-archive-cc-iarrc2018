#include "lanedetector.hpp"

static const int H_slider_max = 360;
static const int S_slider_max = 255;
static const int V_slider_max = 255;
static const int Thresh_max = 255;
static const int Accuracy_max = 100;
static const int F_max = 1000;

static int H_yellow_slider = 110, S_yellow_slider = 49, V_yellow_slider = 102, yel_Thresh_sider = 50;
static int H_white_slider = 123, S_white_slider = 102, V_white_slider = 60, whi_Thresh_sider = 33;
static int A_slider = 13;
static int K_slider = 973;
static int H_yellow = 110, V_yellow = 49, S_yellow = 102, Thresh_yellow = 50;
static int H_white = 123, V_white =102, S_white = 60, Thresh_white = 33;
static int A_value = 50;
static int K_value = 50;
static int Acc_slider = 50;
static int Acc_value = 50;
static int Acc_filt = 8;
static int Acc_filt_slider = 8;

static int R_down_slider = 146, G_down_slider = 82, B_down_slider = 107;
static int R_up_slider = 176, G_up_slider = 255, B_up_slider = 255;
static int R_down = 146, G_down = 82, B_down = 107;
static int R_up = 176, G_up = 255, B_up = 255;

static int H_down_slider = 236, S_down_slider = 0, V_down_slider = 0;
static int H_up_slider = 255, S_up_slider = 255, V_up_slider = 255;
static int H_down = 244, S_down = 0, V_down = 0;
static int H_up = 255, S_up = 255, V_up = 255;

static int H_down_slider_white = 48, S_down_slider_white = 104, V_down_slider_white = 94;
static int H_up_slider_white = 143, S_up_slider_white = 207, V_up_slider_white = 183;
static int H_down_white = 48, S_down_white = 104, V_down_white = 94;
static int H_up_white = 143, S_up_white = 207, V_up_white = 183;

LaneDetector::LaneDetector()
{

}

void LaneDetector::applyBlur(cv::Mat &input, cv::Mat &output)
{
    cv::GaussianBlur(input, output, cv::Size(3,3), 0, 0);
}

void LaneDetector::UndistXML(cv::Mat &cameraMatrix, cv::Mat &distCoeffs)
{
    std::string filename = "/home/selfie/Desktop/cpp-opencv-app/opencv-app/opencv-app/out_camera_data.xml";
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
    int alpha_ = A_slider;
    double focalLength;
    int dist_ = 500;
    int f_ = K_slider;
    focalLength = (double)f_;

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

    // K - intrinsic matrix
    cv::Mat K = (cv::Mat_<float>(3, 4) <<
        focalLength, 0, w / 2, 0,
        0, focalLength, h / 2, 0,
        0, 0, 1, 0
        );

    cv::Mat transfo = K * (T * (R * A1));

    cv::warpPerspective(frame_in, frame_out, transfo, taille, cv::INTER_CUBIC | cv::WARP_INVERSE_MAP);
}

void LaneDetector::CreateTrackbars()
{
    cv::namedWindow("1.1 Yellow Line", 1);
    cv::namedWindow("1.2 White Line", 1);
    cv::namedWindow("3.1 Yellow Bird Eye", 1);
    cv::namedWindow("3.2 White Bird Eye", 1);
    cv::namedWindow("3.3 Line Accuracy",1);
    cv::namedWindow("5 Cone Detect", 1);

    cv::createTrackbar("A", "3.1 Yellow Bird Eye", &A_slider, Accuracy_max, LaneDetector::on_accuracy_trackbar);
    cv::createTrackbar("K", "3.1 Yellow Bird Eye", &K_slider, F_max, LaneDetector::on_f_trackbar);
    cv::createTrackbar("Acc", "3.1 Yellow Bird Eye", &Acc_slider, 255, LaneDetector::on_acc_trackbar);
    cv::createTrackbar("A", "3.2 White Bird Eye", &A_slider, Accuracy_max, LaneDetector::on_accuracy_trackbar);
    cv::createTrackbar("K", "3.2 White Bird Eye", &K_slider, F_max, LaneDetector::on_f_trackbar);
    cv::createTrackbar("Acc", "3.3 Line Accuracy", &Acc_slider, 255, LaneDetector::on_acc_trackbar);
    cv::createTrackbar("Acc_filt", "3.3 Line Accuracy", &Acc_filt_slider, 20, LaneDetector::on_acc_filt_trackbar);

    cv::createTrackbar("Rdown", "5 Cone Detect", &R_down_slider, 255, LaneDetector::on_Rdown_trackbar);
    cv::createTrackbar("Rup", "5 Cone Detect", &R_up_slider, 255, LaneDetector::on_Rup_trackbar);
    cv::createTrackbar("Gdown", "5 Cone Detect", &G_down_slider, 255, LaneDetector::on_Gdown_trackbar);
    cv::createTrackbar("Gup", "5 Cone Detect", &G_up_slider, 255, LaneDetector::on_Gup_trackbar);
    cv::createTrackbar("Bdown", "5 Cone Detect", &B_down_slider, 255, LaneDetector::on_Bdown_trackbar);
    cv::createTrackbar("Bup", "5 Cone Detect", &B_up_slider, 255, LaneDetector::on_Bup_trackbar);

    cv::createTrackbar("Hdown", "1.1 Yellow Line", &H_down_slider, 255, LaneDetector::on_Hdown_trackbar);
    cv::createTrackbar("Hup", "1.1 Yellow Line", &H_up_slider, 255, LaneDetector::on_Hup_trackbar);
    cv::createTrackbar("Sdown", "1.1 Yellow Line", &S_down_slider, 255, LaneDetector::on_Sdown_trackbar);
    cv::createTrackbar("Sup", "1.1 Yellow Line", &S_up_slider, 255, LaneDetector::on_Sup_trackbar);
    cv::createTrackbar("Vdown", "1.1 Yellow Line", &V_down_slider, 255, LaneDetector::on_Vdown_trackbar);
    cv::createTrackbar("Vup", "1.1 Yellow Line", &V_up_slider, 255, LaneDetector::on_Vup_trackbar);

    cv::createTrackbar("Hdown", "1.2 White Line", &H_down_slider_white, 255, LaneDetector::on_Hdown_trackbar_white);
    cv::createTrackbar("Hup", "1.2 White Line", &H_up_slider_white, 255, LaneDetector::on_Hup_trackbar_white);
    cv::createTrackbar("Sdown", "1.2 White Line", &S_down_slider_white, 255, LaneDetector::on_Sdown_trackbar_white);
    cv::createTrackbar("Sup", "1.2 White Line", &S_up_slider_white, 255, LaneDetector::on_Sup_trackbar_white);
    cv::createTrackbar("Vdown", "1.2 White Line", &V_down_slider_white, 255, LaneDetector::on_Vdown_trackbar_white);
    cv::createTrackbar("Vup", "1.2 White Line", &V_up_slider_white, 255, LaneDetector::on_Vup_trackbar_white);
}

void LaneDetector::on_yellow_H_trackbar(int, void*)
{
        H_yellow = H_yellow_slider;
}
void LaneDetector::on_white_H_trackbar(int, void*)
{
        H_white = H_white_slider;
}
void LaneDetector::on_yellow_S_trackbar(int, void*)
{
        S_yellow = S_yellow_slider;
}
void LaneDetector::on_white_S_trackbar(int, void*)
{
        S_white = S_white_slider;
}
void LaneDetector::on_yellow_V_trackbar(int, void*)
{
        V_yellow = V_yellow_slider;
}
void LaneDetector::on_white_V_trackbar(int, void*)
{
        V_white = V_white_slider;
}
void LaneDetector::on_yellow_thresh_trackbar(int, void*)
{
        Thresh_yellow = yel_Thresh_sider;
}
void LaneDetector::on_white_thresh_trackbar(int, void*)
{
        Thresh_white = whi_Thresh_sider;
}
void LaneDetector::on_accuracy_trackbar(int, void*)
{
        A_value = A_slider;
}
void LaneDetector::on_f_trackbar(int, void*)
{
        K_value = K_slider;
}
void LaneDetector::on_acc_trackbar(int, void*)
{
        Acc_value = Acc_slider;
}
void LaneDetector::on_acc_filt_trackbar(int, void*)
{
        Acc_filt = Acc_filt_slider;
}
void LaneDetector::on_Rdown_trackbar(int, void*)
{
        R_down = R_down_slider;
}
void LaneDetector::on_Rup_trackbar(int, void*)
{
        R_up = R_up_slider;
}
void LaneDetector::on_Gdown_trackbar(int, void*)
{
        G_down = G_down_slider;
}
void LaneDetector::on_Gup_trackbar(int, void*)
{
        G_up = G_up_slider;
}
void LaneDetector::on_Bdown_trackbar(int, void*)
{
        B_down = B_down_slider;
}
void LaneDetector::on_Bup_trackbar(int, void*)
{
        B_up = B_up_slider;
}
void LaneDetector::on_Hdown_trackbar(int, void*)
{
        H_down = H_down_slider;
}
void LaneDetector::on_Hup_trackbar(int, void*)
{
        H_up = H_up_slider;
}
void LaneDetector::on_Sdown_trackbar(int, void*)
{
        S_down = S_down_slider;
}
void LaneDetector::on_Sup_trackbar(int, void*)
{
        S_up = S_up_slider;
}
void LaneDetector::on_Vdown_trackbar(int, void*)
{
        V_down = V_down_slider;
}
void LaneDetector::on_Vup_trackbar(int, void*)
{
        V_up = V_up_slider;
}

//
void LaneDetector::on_Hdown_trackbar_white(int, void*)
{
        H_down_white = H_down_slider_white;
}
void LaneDetector::on_Hup_trackbar_white(int, void*)
{
        H_up_white = H_up_slider_white;
}
void LaneDetector::on_Sdown_trackbar_white(int, void*)
{
        S_down_white = S_down_slider_white;
}
void LaneDetector::on_Sup_trackbar_white(int, void*)
{
        S_up_white = S_up_slider_white;
}
void LaneDetector::on_Vdown_trackbar_white(int, void*)
{
        V_down_white = V_down_slider_white;
}
void LaneDetector::on_Vup_trackbar_white(int, void*)
{
        V_up_white = V_up_slider_white;
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

void LaneDetector::colorTransform(cv::Mat &input, cv::Mat &output)
{
    cv::cvtColor(input, input, cv::COLOR_RGB2GRAY);
    cv::threshold(input, output, 20, 255, 1);
}

void LaneDetector::detectLine(cv::Mat &input, std::vector<std::vector<cv::Point>> &output)
{
    std::vector<cv::Vec4i> hierarchy;

    output.clear();

    cv::findContours(input, output, hierarchy, CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE, cv::Point(0, 0));
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

void LaneDetector::ConeDetection(cv::Mat frame_in, cv::Mat &frame_out, std::vector<cv::Point> &conePoints)
{
    cv::Mat image_HSV, img_thresh_low,img_thresh_high, result, frame_tmp;
    std::vector<cv::Vec4i> hierarchy;
    std::vector<std::vector<cv::Point>> contours;

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

    cv::Mat drawing = cv::Mat::zeros(result.size(), CV_8UC3);

    std::vector<cv::Rect> boundRect(contours.size());
    for (int i = 0; i < contours.size(); i++)
    {
        cv::drawContours(drawing, contours, i, cv::Scalar(255, 0, 0), 2, 7, hierarchy, 0, cv::Point());
        boundRect[i] = cv::boundingRect(cv::Mat(contours[i]));
        cv::rectangle(drawing, boundRect[i].tl(), boundRect[i].br(), CV_RGB(200, 200, 0), 2, 8, 0);

        std::string label = "cone";
        cv::putText(drawing, label, boundRect[i].tl() + cv::Point(0, -5), CV_FONT_HERSHEY_DUPLEX, 0.4, CV_RGB(0, 255, 0), 1.0);
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
    cv::Mat poly = cv::Mat::zeros(drawing.rows, drawing.cols, CV_8UC3);
    cv::Mat dst;
    cv::fillPoly(poly, ppt, npt, 1, CV_RGB(0, 0 , 255), 4);

    cv::addWeighted(drawing, 0.7, poly, 0.3, 0.0, dst);
    dst.copyTo(frame_out);
}
