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
#include "include/uart_class.cpp"
#include <wiringPi.h>
//#include "include/streaming.cpp"

#define CAMERA_INDEX 0
#define CAM_RES_X 640
#define CAM_RES_Y 360
#define FRAME_TIME 30

void push_new_data_to_UART();
void U_thread(UART &uart);

bool close_app = false;

int detected_middle_pos_near = 320;
int detected_middle_pos_far = 320;
int left_lane_angle_st = 90;
int left_lane_angle_rad = 3.1415/2;
int right_lane_angle_st = 90;
int right_lane_angle_rad = 3.1415/2;
char flags_to_UART = 0b00000000;

LineDetector lineDetector;
Watchdog watchdog;
UART uart_1;

int main()
{    
    //Get access to memmory shared with watchdog app
    watchdog.get_access();

    //===========Data Acquisition Sector==========

    //CAMERA RGB and YUYV
    cv::VideoCapture camera;
    camera.open(CAMERA_INDEX, cv::CAP_V4L2);

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
        camera.set(cv::CAP_PROP_FRAME_WIDTH, 640);
        camera.set(cv::CAP_PROP_FRAME_HEIGHT, 360);
        camera.set(cv::CAP_PROP_MODE, cv::CAP_MODE_YUYV);
		camera.set(cv::CAP_PROP_CONVERT_RGB, false);
        std::cout << "MODE: " << camera.get(cv::CAP_PROP_MODE) << std::endl;
		std::cout << "RGB: " << camera.get(cv::CAP_PROP_CONVERT_RGB) << std::endl;
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


	//GPIO
	wiringPiSetup();
	pinMode(23, INPUT);
	pinMode(4, INPUT);

	bool GPIO_input[2] = {0,0};
	bool GPIO_handled[2] = {0,0};
	
	//getchar();

    //GUI
    char keypressed;
	bool show_mask = false;

    //===========Data Acquisition Sector==========

    cv::Mat frame(CAM_RES_Y, CAM_RES_X, CV_8UC4);
    cv::Mat frame_gray(CAM_RES_Y, CAM_RES_X, CV_8UC1);
	//cv::Mat frame_gray_bird(CAM_RES_Y, CAM_RES_X, CV_8UC1);
    cv::Mat frame_edges_masked(CAM_RES_Y, CAM_RES_X, CV_8UC1);

    //===========Data Acquisition Sector==========

    cv::Mat mask = cv::Mat::zeros(cv::Size(640, 360), CV_8UC1);
    cv::Point points[6] =
    {
        cv::Point(0, 320),
		cv::Point(0, 240),
        cv::Point(60, 140),
        cv::Point(530, 170),
		cv::Point(639, 240),
        cv::Point(639, 320)
    };
    cv::fillConvexPoly(mask, points, 6, cv::Scalar(255, 0, 0));

    std::vector<cv::Mat> frame_split_vec(4);

    cv::Mat frame_thresh(CAM_RES_Y, CAM_RES_X, CV_8UC1);
    cv::Mat frame_edges(CAM_RES_Y, CAM_RES_X, CV_8UC1);
    cv::Mat frame_lines(CAM_RES_Y, CAM_RES_X, CV_8UC1);
	cv::Mat frame_data(CAM_RES_Y, CAM_RES_X, CV_8UC3);

    //cv::namedWindow("Vision", 1);
    //cv::moveWindow("Vision", 0, 0);
    cv::namedWindow("Vision GRAY", 1);
    cv::moveWindow("Vision GRAY", 400, 0);
    //cv::namedWindow("Thresh", 1);
    //cv::moveWindow("Thresh", 1280, 0);
    //cv::namedWindow("Edges", 1);
    //cv::moveWindow("Edges", 640, 0);
    cv::namedWindow("Masked", 1);
    cv::moveWindow("Masked", 640, 460);
    //cv::namedWindow("Lines", 1);
    //cv::moveWindow("Lines", 640, 400);
	cv::namedWindow("Data", 1);
    cv::moveWindow("Data", 0, 510);
 

    int thresh_value = 210;
	int P_min_votes = 10;
	int P_max_gap = 2;
	int P_min_len = 15;
    
	cv::createTrackbar("Thresh", "Vision GRAY", &thresh_value, 255);
	cv::createTrackbar("P_min_votes", "Masked", &P_min_votes, 60);
	cv::createTrackbar("P_max_gap", "Masked", &P_max_gap, 60);
	cv::createTrackbar("P_min_len", "Masked", &P_min_len, 60);

    //Threads
    //std::thread UART_thread(U_thread, std::ref(uart_1), std::ref(gpio), std::ref(pushButtonValue), std::ref(redLEDValue), std::ref(yellowLEDValue), std::ref(greenLEDValue));

    //FPS
    struct timespec start, end;
    unsigned short licznik_czas = 0;
    double seconds = 0;
    double fps = 0;
    //FPS

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
        camera >> frame;
        cv::split(frame, frame_split_vec);
        frame_gray = frame_split_vec[0];

        //RGB --> GRAY
        //camera >> frame;
        //cv::cvtColor(frame, frame_split_vec[0], cv::COLOR_BGR2GRAY);

        //Read from file
        //Odkomentować na początku main()

        //===========Data Acquisition Sector==========

        lineDetector.applyBlur(frame_gray, frame_gray);
		//lineDetector.applyBirdEye(frame_gray, frame_gray_bird);
		//frame_gray = frame_gray_bird;
        lineDetector.edgeDetect(frame_gray, frame_thresh, frame_edges, thresh_value);
        lineDetector.applyMask(frame_edges, mask, frame_edges_masked);
        lineDetector.detectLines(frame_edges_masked, P_min_votes, P_max_gap, P_min_len);
        
		lineDetector.calculate_all_slopes();
		lineDetector.sort_lines();
		lineDetector.save_for_next_step();	

		lineDetector.parking_line(frame_thresh);
		//lineDetector.cross_line(frame_thresh);

		lineDetector.draw_data(frame_data);

		lineDetector.send_data_to_main(detected_middle_pos_near, left_lane_angle_st, right_lane_angle_st, flags_to_UART);

        push_new_data_to_UART();

		uart_1.send_data();

		//GPIO
		GPIO_input[0] = digitalRead(4);	
		GPIO_input[1] = digitalRead(6);
		
		std::cout << "PIN 18: " << GPIO_input[0] << std::endl;
		std::cout << "PIN 26: " << GPIO_input[1] << std::endl;
		
		if(GPIO_input[0])
		{
			if(!GPIO_handled[0])
			{
				lineDetector.restart_lane_detection();
				GPIO_handled[0] = true;
			}
		}
		else
		{
			GPIO_handled[0] = false;
		}
		
		if(GPIO_input[1] & !GPIO_handled[1])
		{
			if(!GPIO_handled[1])
			{
				lineDetector.change_lane();
				GPIO_handled[1] = true;
			}
		}	
		else
		{
			GPIO_handled[1] = false;
		}

		if(show_mask)
		{
			cv::fillConvexPoly(frame_gray, points, 6, cv::Scalar(255, 0, 0));
		}

        //cv::imshow("Vision", frame);
        cv::imshow("Vision GRAY", frame_gray);
        //cv::imshow("Thresh", frame_thresh);
        //cv::imshow("Edges", frame_edges);
        cv::imshow("Masked", frame_edges_masked);
        //cv::imshow("Lines", frame_lines);
		cv::imshow("Data", frame_data);

		lineDetector.display_last_middle();

		std::cout << "Time in seconds [1000 frames]: " << seconds << std::endl;
        std::cout << "Estimated frames per second : " << fps << std::endl;

		if(keypressed == -1)
			std::cout << "Keypressed: NONE "<< std::endl;
		else
			std::cout << "Keypressed: " << keypressed << std::endl;

        keypressed = (char)cv::waitKey(FRAME_TIME);

        if( keypressed == 27 )
            break;

        switch(keypressed)
        {
        //Switch on/off mask displaing
        case 'q':
            if( show_mask )
                show_mask = false;
            else
                show_mask = true;
            break;

        //Send fixed data, do not proceed incoming frames
        case 'z':
            while(true)
			{
				 //Set flag for watchdog
        		watchdog.push_flag(1);
				
				camera >> frame;

				uart_1.unia_danych.dane.sync_byte = 0xff;
				uart_1.unia_danych.dane.data_0 = 320;
				uart_1.unia_danych.dane.data_1 = 90;
				uart_1.unia_danych.dane.data_2 = 90;
				uart_1.unia_danych.dane.flags = 0b11000000;
				uart_1.unia_danych.dane.end_byte = 0xfe;

				uart_1.send_data();

				std::cout << "\033[2J\033[1;1H";
				std::cout << "Fixed data sent" << std::endl;

				std::cout << "==========UART==========" << std::endl;
				std::cout << "Srodek:    " << uart_1.unia_danych.dane.data_0 << std::endl;
				std::cout << "Kat lewo:  " << uart_1.unia_danych.dane.data_1 << std::endl;
				std::cout << "Kat prawo: " << uart_1.unia_danych.dane.data_2 << std::endl;
				std::cout << "==========UART==========" << std::endl;

				cv::imshow("Vision GRAY", frame_gray);
				cv::imshow("Masked", frame_edges_masked);
				cv::imshow("Data", frame_data);

				keypressed = (char)cv::waitKey(FRAME_TIME);

		        if( keypressed == 27 || keypressed == 'z')
		            break;
			}
            break;

        //Force lane change
        case 'c':
            lineDetector.change_lane();
            break;

        //Manual restart of vision variables
        case 'r':
            lineDetector.restart_lane_detection();
            break;
        }

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
    }

    close_app = true;
    
	camera.release();
    
	//UART_thread.join();
    
	watchdog.push_flag(2);

    return 0;
}

void U_thread(UART &uart)
{
    while(!close_app)
    {
        uart.send_data();
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


