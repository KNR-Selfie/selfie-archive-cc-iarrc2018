#include "include/camera.cpp"
#include "include/GUI.cpp"
#include "include/lane_detection.cpp"
#include "include/sign_detection.cpp"
#include "include/UART.cpp"

#define CAM_INDEX 0
#define CAM_WIDTH 640
#define CAM_HEIGHT 480

int main(int argc, char* argv[])
{
    //Setup
    if(!start_camera(CAM_INDEX, CAM_WIDTH, CAM_HEIGHT))
    {
        return 0;
    }

    //Main loop
    while(true)
    {

    }

    return 0;
}
