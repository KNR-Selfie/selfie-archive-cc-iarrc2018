#include <iostream>
#include <thread>
#include <chrono>

#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"

#include <include/usb.hpp>

USB Usb_STM;
USB Usb_LIDAR;

int main()
{
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

    return 0;
}
