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

#include "include/gpio_class.cpp"
#include "include/watchdog_class.cpp"
#include "include/line_class.cpp"

int main()
{
    //Get access to memmory shared with watchdog app
    Watchdog watchdog;
    watchdog.get_access();

    //Access to GPIO pins
    GPIO gpio;

    //All stuff for line detections
    LineDetector lineDetector;

    cv::VideoCapture camera;
    camera.open(-1);

    if (!camera.isOpened())
    {
        std::cout << "Error could not open any camera!" << std::endl
                  << "Closing app!" << std::endl;

        camera.release();
        return -1;
    }
    else
    {
        camera.set(CV_CAP_PROP_FRAME_WIDTH, 640);
        camera.set(CV_CAP_PROP_FRAME_HEIGHT, 360);
    }

    char keypressed;

    cv::Mat frame;

    cv::namedWindow("Vision", 1);
    cv::moveWindow("Vision", 0, 500);

    //FPS
    struct timespec start, end;
    unsigned short licznik_czas = 0;
    double seconds = 0;
    double fps = 0;
    //FPS

    while(true)
    {
        //FPS
        if(licznik_czas == 0)
        {
            clock_gettime(CLOCK_MONOTONIC, &start);
        }
        //FPS

        //Set flag for watchdog
        watchdog.push_flag(1);

        camera >> frame;

        cv::imshow("Vision", frame);

        keypressed = (char)cv::waitKey(1);

        if( keypressed == 27 )
            break;

        if(licznik_czas > 1000)
        {
            licznik_czas = 0;
            clock_gettime(CLOCK_MONOTONIC, &end);
            seconds = (end.tv_sec - start.tv_sec);
            fps  =  1 / (seconds / 1000);

            std::cout << "        Time in seconds [1000 frames]: " << seconds << std::endl;
            std::cout << "        Estimated frames per second : " << fps << std::endl;
        }
        else
        {
                licznik_czas++;
        }
    }

    //Tell watchdog to stop work
    watchdog.push_flag(2);
    camera.release();
    return 0;
}
