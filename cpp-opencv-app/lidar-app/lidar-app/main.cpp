#include <stdlib.h>
#include <iostream>
#include <cstdio>
#include <thread>
#include <chrono>
#include <math.h>

#include "urg_sensor.h"
#include "urg_utils.h"

#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"

#include "include/data_process.h"
#include "include/sharedmemory.hpp"

#define FRAME_TIME 100
#define SHM_SIZE 10000

//#define DEBUG_MODE
#define TEST_SHM

#ifndef DEBUG_MODE
#undef TEST_SHM
#endif

SharedMemory shm_lidar(50001, SHM_SIZE);

int main()
{
    // Handle for lidar operations
    urg_t urg;

    // Serial communication parameters
    const char connect_device[] = "/dev/ttyACM0";
    const long connect_baudrate = 115200;

    // Try to start communication
    if(urg_open(&urg, URG_SERIAL, connect_device, connect_baudrate) < 0)
    {
        std::cout << "Can not open serial communication with sensor" << std::endl;
        std:: cout << "Closing app" << std:: endl;
        return -1;
    }
    else
    {
        std::cout << "Connected" << std::endl;
    }

    // Check sensor
    if(!(urg_is_stable(&urg)))
    {
        std::cout << "Sensor is unstable and can not do measurement!" << std::endl;
        std::cout << "Closing app" << std::endl;
        return -2;
    }

    shm_lidar.init();

    // Custom data
    LidarReading raw_data;
    LidarReading filtered_data_1;
    LidarReading filtered_data_2;
    LidarReading filtered_data_3;

    // Allocate memmory for data
    int data_size = urg_max_data_size(&urg);
    raw_data.data = (long *)malloc(data_size * sizeof(long));

    // Calculate angle offset for OpenCV drawing
    urg_start_measurement(&urg, URG_DISTANCE, 1, 0);
    int n = urg_get_distance(&urg, raw_data.data, NULL);
    int angle_offset = 150 - ((data_size - n) * 0.36) / 4;

#ifdef DEBUG_MODE
    // OpenCV variables
    cv::Mat frame_raw = cv::Mat(FRAME_X, FRAME_Y, CV_8UC3);
    cv::Mat frame_filtered = cv::Mat(FRAME_X, FRAME_Y, CV_8UC3);
    cv::Mat frame_test(FRAME_X, FRAME_Y, CV_8UC3);
#endif
#ifdef DEBUG_MODE
    // Wait for user
    std::cout << "Configuration done, press enter to read data" << std::endl;
    getchar();
#endif

    int left_angle = 0;
    int right_angle = 450;
    int closest = 75;
    int farthest = 400;
    int scale = 1;
    int thresh_single = 10;

#ifdef DEBUG_MODE
    //Windows
    cv::namedWindow("RAW", cv::WINDOW_AUTOSIZE);
    cv::namedWindow("FILTERED", cv::WINDOW_AUTOSIZE);

#ifdef TEST_SHM
    cv::namedWindow("TEST", cv::WINDOW_AUTOSIZE);
#endif

    cv::moveWindow("RAW", 0, 0);
    cv::moveWindow("FILTERED", 920, 0);

    // Trackbar
    cv::createTrackbar("Scale", "RAW", &scale, 20);
    cv::createTrackbar("Left", "FILTERED", &left_angle, 270);
    cv::createTrackbar("Right", "FILTERED", &right_angle, 450);
    cv::createTrackbar("Closest", "FILTERED", &closest, 1000);
    cv::createTrackbar("Farthest", "FILTERED", &farthest, 5000);
    cv::createTrackbar("Single", "FILTERED", &thresh_single, 100);
#endif

    // Processing loop
    while(true)
    {
        // Start sensor
        if(urg_start_measurement(&urg, URG_DISTANCE, 1, 0) < 0)
        {
            std::cout << "Can not start measurement" << std::endl;
            std:: cout << "Closing app" << std:: endl;
            return -3;
        }

        // Get new data from sensor
        n = urg_get_distance(&urg, raw_data.data, NULL);

        // Change coordinates from polar to cartesian
        polar_to_cartesian(raw_data, n, angle_offset, scale);

        // Cut data reflected from the car
        filter_angle(raw_data, filtered_data_1, left_angle, right_angle);

        // Cut data to far and to close
        filter_distance(filtered_data_1, filtered_data_2, closest, farthest, scale);

        // Cut static in dataset
        filter_single_points(filtered_data_2, thresh_single, filtered_data_3);

#ifdef DEBUG_MODE
        // Draw data
        draw_data(frame_raw, raw_data);
        draw_data(frame_filtered, filtered_data_3);
        draw_boundaries(frame_filtered, filtered_data_1, closest, farthest, scale);
#endif
        // Send data to SHM
        shm_lidar.push_data(filtered_data_3.pos);

#ifdef TEST_SHM
        //Read from SHM
        shm_lidar.pull_data(frame_test);
#endif
#ifdef DEBUG_MODE
        // Show data in window
        cv::imshow("RAW", frame_raw);
        cv::imshow("FILTERED", frame_filtered);

#ifdef TEST_SHM
        cv::imshow("TEST", frame_test);
#endif
        // Clear window for next frame data
        frame_raw = cv::Mat::zeros(FRAME_X, FRAME_Y, CV_8UC3);
        frame_filtered = cv::Mat::zeros(FRAME_X, FRAME_Y, CV_8UC3);
        frame_test = cv::Mat::zeros(FRAME_X, FRAME_Y, CV_8UC3);
#endif
#ifdef DEBUG_MODE
        // Check user input
        char key_input = cv::waitKey(FRAME_TIME);
        if(key_input == 27)
            break;
#endif
    }

    // Stop sensor
    urg_stop_measurement(&urg);

    // Disconnect from the sensor
    urg_close(&urg);
    return 0;
}
