/*
#include <iostream>
#include <string>
#include <chrono>
#include <thread>
#include <mutex>

#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#define CAMERA_INDEX 1
#define CAM_RES_X 640
#define CAM_RES_Y 360
#define FRAME_TIME 30

#define FPS_COUNT

class SafeMat
{
private:
    std::mutex mu_cam;
    std::mutex mu_str;
public:
    cv::Mat safe_frame;

public:
    SafeMat() {}

    void read_write(cv::Mat &in_out, bool mode) // 0 - read, 1 - write
    {
        std::lock_guard<std::mutex> locker(mu_cam);
        if(mode)
        {
            in_out.copyTo(safe_frame);
        }
        else
        {
            safe_frame.copyTo(in_out);
        }
    }

    void print(std::string str)
    {
        std::lock_guard<std::mutex> locker(mu_str);
        std::cout << str << std::endl;
    }
};

bool run_thread = true;
cv::Mat frame_cam(CAM_RES_Y, CAM_RES_X, CV_8UC3);
cv::Mat frame_main(CAM_RES_Y, CAM_RES_X, CV_8UC3);

void camera_thread(cv::VideoCapture camera, SafeMat &safe_mat)
{
    while(run_thread)
    {
        // Take new frame to local cv::Mat
        camera >> frame_cam;
        // Save local frame to shared safe frame
        safe_mat.read_write(frame_cam, true);
        // Print
        safe_mat.print("Thread");
        // Wait
        std::this_thread::sleep_for(std::chrono::milliseconds(FRAME_TIME));
    }
    camera.release();
}

int main()
{
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

        // Set coding type of frames (Kurokesu)
        camera.set(cv::CAP_PROP_CONVERT_RGB, true);
    }

#ifdef FPS_COUNT
    // FPS
    struct timespec start, end;
    unsigned int frame_count = 0;
    float seconds = 0;
    float fps = 0;
#endif

    // UI
    char keypressed;

    SafeMat safe_mat;

    std::thread thread_1(camera_thread, std::move(camera), std::ref(safe_mat));
    std::this_thread::sleep_for(std::chrono::milliseconds(1000));

    // Main loop
    while(true)
    {
#ifdef FPS_COUNT
        // FPS
        if(frame_count == 0)
        {
            clock_gettime(CLOCK_MONOTONIC, &start);
        }
#endif
        // Take new frame from shared safe space
        safe_mat.read_write(frame_main, false);

        // Show local frame
        cv::imshow("Camera", frame_main);

        safe_mat.print("Main");

        // Get input from user
        keypressed = (char)cv::waitKey(FRAME_TIME);

        // Process given input
        if( keypressed == 27 )
            break;

        if(keypressed == 'p')
            run_thread = false;

#ifdef FPS_COUNT
        // FPS
        if(frame_count > 1000)
        {
            frame_count = 0;
            clock_gettime(CLOCK_MONOTONIC, &end);
            seconds = (end.tv_sec - start.tv_sec);
            fps  =  1 / (seconds / 1000);

            std::cout <<"FPS: " << fps << std::endl;
        }
        else
        {
            frame_count++;
        }
#endif
    }

    run_thread = false;
    thread_1.join();
    return 0;
}

*/
