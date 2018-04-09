#include <iostream>
#include <thread>
#include <chrono>

#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"

#include <include/usb.hpp>

#define CAMERA_INDEX 0
#define CAM_RES_X 640
#define CAM_RES_Y 360
#define FRAME_TIME 30

USB Usb_STM;
USB Usb_LIDAR;

int main()
{
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

    while(true)
    {
        // Get new frame from camera
        camera >> frame;

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
    }

    return 0;
}
