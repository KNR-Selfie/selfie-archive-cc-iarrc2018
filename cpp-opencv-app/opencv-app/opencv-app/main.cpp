#include <iostream>
#include <thread>
#include <chrono>

#include <include/usb.hpp>
#include <include/lanedetector.hpp>

// Race mode ---> fps boost
// Debug mode --> display data
//#define RACE_MODE
#define DEBUG_MODE

#define CAMERA_INDEX 0
#define CAM_RES_X 640
#define CAM_RES_Y 360
#define FRAME_TIME 30

USB Usb_STM;
USB Usb_LIDAR;
LaneDetector laneDetector;

int main()
{
#ifdef DEBUG_MODE
    //FPS
    struct timespec start, end;
    unsigned short licznik_czas = 0;
    float seconds = 0;
    float fps = 0;
#endif

    // Declaration of cv::MAT variables
    cv::Mat frame(CAM_RES_Y, CAM_RES_X, CV_8UC4);

    // STM communication init
    if(Usb_STM.init(B1152000) < 0)
    {
        std::cout << "Closing app!" << std::endl;
        return -1;
    }

    // Lidar communication init
    if(Usb_LIDAR.init(B115200) < 0)
    {
        std::cout << "Closing app!" << std::endl;
        return -1;
    }

    // Camera init
    cv::VideoCapture camera;
    camera.open(CAMERA_INDEX, cv::CAP_V4L2);

    // Check if camera is opened propely
    if (!camera.isOpened())
    {
        std::cout << "Error could not open camera on port: " << CAMERA_INDEX << std::endl
                  << "Closing app!" << std::endl;

        camera.release();
        return 0;
    }
    else
    {
        // Set resolution
        camera.set(cv::CAP_PROP_FRAME_WIDTH, CAM_RES_X);
        camera.set(cv::CAP_PROP_FRAME_HEIGHT, CAM_RES_Y);

        //Set and check decoding of frames
        camera.set(cv::CAP_PROP_CONVERT_RGB, true);
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

        return 0;
    }
*/

    while(true)
    {
#ifdef DEBUG_MODE
        //FPS
        if(licznik_czas == 0)
        {
            clock_gettime(CLOCK_MONOTONIC, &start);
        }
        //FPS
#endif

        // Get new frame from camera
        camera >> frame;

#ifdef DEBUG_MODE
        // Display info on screen
        cv::imshow("Camera", frame);

        // Get input from user
        char keypressed = (char)cv::waitKey(FRAME_TIME);

        // Process given input
        if( keypressed == 27 )
            break;

        switch(keypressed)
        {
        case '1':

            break;

        case '2':

            break;

        default:

            break;
        }

        if(licznik_czas > 100)
        {
            licznik_czas = 0;
            clock_gettime(CLOCK_MONOTONIC, &end);
            seconds = (end.tv_sec - start.tv_sec);
            fps  =  1 / (seconds / 100);
            std::cout << "FPS: " << fps << std::endl;
        }
        else
        {
            licznik_czas++;
        }
#endif

    }

    return 0;
}
