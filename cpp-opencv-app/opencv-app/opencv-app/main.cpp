#include <iostream>
#include <thread>
#include <chrono>
#include <mutex>

#include <include/usb.hpp>
#include <include/sharedmemory.hpp>
#include <include/lanedetector.hpp>

// Race mode ---> fps boost
// Debug mode --> display data
//#define RACE_MODE
#define DEBUG_MODE

#define CAMERA_INDEX 0
#define CAM_RES_X 640
#define CAM_RES_Y 360
#define FRAME_TIME 30

// Handlers for custom classes
USB Usb_STM;
USB Usb_LIDAR;
LaneDetector laneDetector;

// Variables for communication between threads
bool close_app = false;
std::mutex mu;

// Function declarations
void read_from_STM_thread(USB &STM);

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
/*
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
*/
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
    // Start threads
    std::thread USB_STM_in_thread(read_from_STM_thread, std::ref(Usb_STM));

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
/*
        // Process frame
        laneDetector.applyBlur();
        laneDetector.colorTransform();
        laneDetector.edgeDetect();

        laneDetector.detectLine();
        laneDetector.detectLine();

        laneDetector.drawData();
        laneDetector.drawData();
*/
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

    close_app = true;
    USB_STM_in_thread.join();
    std::cout << "KK skończyłęm!" << std::endl;
    return 0;
}

void read_from_STM_thread(USB &STM)
{
    while(!close_app)
    {
        // Read new data if available
        if(STM.read_one_chunk())
        {
            // Process data

            //Save new data
            std::unique_lock<std::mutex> locker(mu);

            locker.unlock();
        }
        else
        {
            // Wait 5ms
            std::this_thread::sleep_for(std::chrono::milliseconds(5));
        }
    }
}
