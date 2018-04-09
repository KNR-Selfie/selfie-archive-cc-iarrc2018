#include <iostream>
#include <thread>
#include <chrono>

#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"

#include <include/usb.hpp>

#define CAM_RES_X 640
#define CAM_RES_Y 360
#define FRAME_TIME 30

USB Usb_STM;
USB Usb_LIDAR;

int main()
{
    cv::Mat frame(CAM_RES_Y, CAM_RES_X, CV_8UC4);

    if(Usb_STM.init(B1152000) < 0)
    {
        std::cout << "Closing app!" << std::endl;
        return -1;
    }

    if(Usb_LIDAR.init(B115200) < 0)
    {
        std::cout << "Closing app!" << std::endl;
        return -1;
    }

    while(true)
    {
        cv::imshow("Camera", frame);

        keypressed = (char)cv::waitKey(FRAME_TIME);

        if( keypressed == 27 )
            break;

        switch(keypressed)
        {
        case 'q':

            break;

        case 's':

            break;
        }
    }

    return 0;
}
