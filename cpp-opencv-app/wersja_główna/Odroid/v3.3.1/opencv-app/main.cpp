#include "opencv2/core/core.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/opencv.hpp"

#include <iostream>
#include <stdio.h>
#include <string>
#include <vector>
#include <time.h>
#include <thread>
#include <mutex>
#include <atomic>

#include "include/watchdog_class.cpp"
#include "include/line_class.cpp"
#include "include/gpio_class.cpp"
#include "include/uart_class.cpp"

#define CAMERA_INDEX 0
#define CAM_RES_X 640
#define CAM_RES_Y 360
#define COMPARED_LEVEL 100
#define FRAME_TIME 50

void push_new_data_to_UART();
void U_thread(UART &uart, GPIO &gpio, unsigned int &pushButtonValue, unsigned int &redLEDValue, unsigned int &yellowLEDValue, unsigned int &greenLEDValue);
void check_and_write_gpios(GPIO &gpio, unsigned int &pushButtonValue, unsigned int &redLEDValue, unsigned int &yellowLEDValue, unsigned int &greenLEDValue);

bool close_app = false;

int detected_middle_pos_near = 320;
int detected_middle_pos_far = 320;
int left_lane_angle_st = 90;
int left_lane_angle_rad = 3.1415/2;
int right_lane_angle_st = 90;
int right_lane_angle_rad = 3.1415/2;
int8_t flags_to_UART = 0b00000000;

unsigned int pushButtonValue = 0;
unsigned int redLEDValue = 0;
unsigned int yellowLEDValue = 0;
unsigned int greenLEDValue = 0;

LineDetector lineDetector;
Watchdog watchdog;
UART uart_1;
GPIO gpio;

int main()
{    
    //Get access to memmory shared with watchdog app
    watchdog.get_access();

    //===========Data Acquisition Sector==========

    //CAMERA RGB and YUYV
    cv::VideoCapture camera;
    camera.open(CAMERA_INDEX);

    if (!camera.isOpened())
    {
        std::cout << "Error could not open any camera!" << std::endl
                  << "Closing app!" << std::endl;

        close_app = true;
        watchdog.push_flag(2);
        camera.release();
        return 0;
    }
    else
    {
        camera.set(CV_CAP_PROP_FRAME_WIDTH, 640);
        camera.set(CV_CAP_PROP_FRAME_HEIGHT, 360);
        camera.set(CV_CAP_PROP_MODE, 3);
        std::cout << camera.get(CV_CAP_PROP_MODE);
    }
/*
    //Read from file
    cv::Mat frame_gray(CAM_RES_Y, CAM_RES_X, CV_8UC1);
    frame_gray = cv::imread("../../data/SELFIE-example-img-prosto.png", CV_LOAD_IMAGE_GRAYSCALE);

    if(!frame_gray.data)
    {
        std::cout << "No data!" << std::endl
                  << "Closing app!" << std::endl;

        close_app = true;
        watchdog.push_flag(2);
        return 0;
    }
*/
    //===========Data Acquisition Sector==========

    //UART

    uart_1.set();

    uart_1.unia_danych.dane.sync_byte = 0xff;
    uart_1.unia_danych.dane.data_0 = 320;
    uart_1.unia_danych.dane.data_1 = 90;
    uart_1.unia_danych.dane.data_2 = 90;
    uart_1.unia_danych.dane.data_3 = 0;
    uart_1.unia_danych.dane.data_4 = 0;
    uart_1.unia_danych.dane.data_5 = 0;
    uart_1.unia_danych.dane.data_6 = 0;
    uart_1.unia_danych.dane.data_7 = 0;
    uart_1.unia_danych.dane.flags = 0b00000000;
    uart_1.unia_danych.dane.end_byte = 0xfe;
/*
    //GPIO
    gpio.Export(gpio.pushButton);
    gpio.Export(gpio.redLED);
    gpio.Export(gpio.yellowLED);
    gpio.Export(gpio.greenLED);

    gpio.SetDirection(gpio.pushButton, input);
    gpio.SetDirection(gpio.redLED, output);
    gpio.SetDirection(gpio.yellowLED, output);
    gpio.SetDirection(gpio.greenLED, output);

    gpio.SetValue(gpio.redLED, low);
    gpio.SetValue(gpio.yellowLED, low);
    gpio.SetValue(gpio.greenLED, low);
*/
    //GUI
    char keypressed;

    //===========Data Acquisition Sector==========

    cv::Mat frame(CAM_RES_Y, CAM_RES_X, CV_8UC4);
    cv::Mat frame_gray(CAM_RES_Y, CAM_RES_X, CV_8UC1);
    cv::Mat frame_edges_masked(CAM_RES_Y, CAM_RES_X, CV_8UC1);

    //===========Data Acquisition Sector==========

    cv::Mat mask = cv::Mat::zeros(cv::Size(640, 360), CV_8UC1);
    cv::Point points[4] =
    {
        cv::Point(10, 320),
        cv::Point(100, 80),
        cv::Point(540, 80),
        cv::Point(630, 320)
    };
    cv::fillConvexPoly(mask, points, 4, cv::Scalar(255, 0, 0));

    std::vector<cv::Mat> frame_split_vec(4);
    cv::Mat frame_thresh(CAM_RES_Y, CAM_RES_X, CV_8UC1);
    cv::Mat frame_edges(CAM_RES_Y, CAM_RES_X, CV_8UC1);
    std::vector<cv::Vec4i> lines;
    cv::Mat frame_lines(CAM_RES_Y, CAM_RES_X, CV_8UC1);

    std::vector<cv::Point> lane_corners;

    cv::namedWindow("Vision", 1);
    cv::moveWindow("Vision", 0, 0);
    cv::namedWindow("Vision GRAY", 1);
    cv::moveWindow("Vision GRAY", 0, 0);
    cv::namedWindow("Thresh", 1);
    cv::moveWindow("Thresh", 640, 0);
    cv::namedWindow("Edges", 1);
    cv::moveWindow("Edges", 640, 0);
    cv::namedWindow("Masked", 1);
    cv::moveWindow("Masked", 0, 400);
    cv::namedWindow("Lines", 1);
    cv::moveWindow("Lines", 640, 400);


    int thresh_value = 210;
    cv::createTrackbar("Thresh", "Vision GRAY", &thresh_value, 255);

    //Threads
    //std::thread UART_thread(U_thread, std::ref(uart_1), std::ref(gpio), std::ref(pushButtonValue), std::ref(redLEDValue), std::ref(yellowLEDValue), std::ref(greenLEDValue));

    //FPS
    struct timespec start, end;
    unsigned short licznik_czas = 0;
    double seconds = 0;
    double fps = 0;
    //FPS

    float av_slope_left_near;
    float av_slope_right_near;

    while(true)
    {
        std::cout << "\033[2J\033[1;1H";

        //FPS
        if(licznik_czas == 0)
        {
            clock_gettime(CLOCK_MONOTONIC, &start);
        }
        //FPS

        //Set flag for watchdog
        watchdog.push_flag(1);

        //===========Data Acquisition Sector==========

        //YUYV --> GRAY
        //camera >> frame;
        //cv::split(frame, frame_split_vec);
        //frame_gray = frame_split_vec[0];

        //RGB --> GRAY
        camera >> frame;
        cv::cvtColor(frame, frame_split_vec[0], cv::COLOR_BGR2GRAY);

        //Read from file
        //Odkomentować na początku main()

        //===========Data Acquisition Sector==========

        lineDetector.applyBlur(frame_split_vec[0], frame_gray);
        lineDetector.edgeDetect(frame_gray, frame_thresh, frame_edges, thresh_value);
        lineDetector.applyMask(frame_edges, mask, frame_edges_masked);
        lineDetector.detectLines(frame_edges_masked, lines);
        lineDetector.drawLines(lines, frame_lines);

        lineDetector.writePoints();

        //lineDetector.get2points(lines, frame_lines, lane_corners);
        //lineDetector.calculateDataToSend(lane_corners, detected_middle_pos_near, detected_middle_pos_far, left_lane_angle_st, right_lane_angle_st, flags_to_UART);

        std::cout << "Linie: " << lines.size() << std::endl;

        //push_new_data_to_UART();

        cv::imshow("Vision", frame);
        cv::imshow("Vision GRAY", frame_gray);
        cv::imshow("Thresh", frame_thresh);
        cv::imshow("Edges", frame_edges);
        cv::imshow("Masked", frame_edges_masked);
        cv::imshow("Lines", frame_lines);

        keypressed = (char)cv::waitKey(FRAME_TIME);
        if( keypressed == 27 )
            break;

        if(licznik_czas > 1000)
        {
            licznik_czas = 0;
            clock_gettime(CLOCK_MONOTONIC, &end);
            seconds = (end.tv_sec - start.tv_sec);
            fps  =  1 / (seconds / 1000);

            
        }
        else
        {
                licznik_czas++;
        }

		std::cout << "        Time in seconds [1000 frames]: " << seconds << std::endl;
            std::cout << "        Estimated frames per second : " << fps << std::endl;
    }

    close_app = true;
/*
    gpio.Unexport(gpio.pushButton);
    gpio.Unexport(gpio.redLED);
    gpio.Unexport(gpio.yellowLED);
    gpio.Unexport(gpio.greenLED);
*/
    camera.release();

    //UART_thread.join();

    watchdog.push_flag(2);
    return 0;
}

void U_thread(UART &uart, GPIO &gpio, unsigned int &pushButtonValue, unsigned int &redLEDValue, unsigned int &yellowLEDValue, unsigned int &greenLEDValue)
{
    while(!close_app)
    {
        uart.send_data();
        check_and_write_gpios(gpio, pushButtonValue, redLEDValue, yellowLEDValue, greenLEDValue);
        std::this_thread::sleep_for(std::chrono::milliseconds(30));
    }
}

void push_new_data_to_UART()
{
    uart_1.unia_danych.dane.data_0 = detected_middle_pos_near;
    uart_1.unia_danych.dane.data_1 = left_lane_angle_st;
    uart_1.unia_danych.dane.data_2 = right_lane_angle_st;
    //uart_1.unia_danych.dane.data_3 = 0;
    //uart_1.unia_danych.dane.data_4 = 0;
    //uart_1.unia_danych.dane.data_5 = 0;
    //uart_1.unia_danych.dane.data_6 = 0;
    //uart_1.unia_danych.dane.data_7 = 0;
    uart_1.unia_danych.dane.flags = flags_to_UART;
}

void check_and_write_gpios(GPIO &gpio, unsigned int &pushButtonValue, unsigned int &redLEDValue, unsigned int &yellowLEDValue, unsigned int &greenLEDValue)
{
    gpio.GetValue(gpio.pushButton, &pushButtonValue);
    std::cout << "       Wartosc przycisku: " << pushButtonValue << std::endl;

    gpio.SetValue(gpio.redLED, redLEDValue);
    gpio.SetValue(gpio.yellowLED, yellowLEDValue);
    gpio.SetValue(gpio.greenLED, greenLEDValue);
}
