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
#include "include/gpio_class.cpp"
#include "include/uart_class.cpp"

void push_new_data_to_UART();
void U_thread(UART &uart, GPIO &gpio, unsigned int &pushButtonValue, unsigned int &redLEDValue, unsigned int &yellowLEDValue, unsigned int &greenLEDValue);
void check_and_write_gpios(GPIO &gpio, unsigned int &pushButtonValue, unsigned int &redLEDValue, unsigned int &yellowLEDValue, unsigned int &greenLEDValue);

bool close_app = false;

int detected_middle_pos = 320;
int left_lane_angle_st = 90;
int left_lane_angle_rad = 3.1415/2;
int right_lane_angle_st = 90;
int right_lane_angle_rad = 3.1415/2;

unsigned int pushButtonValue = 0;
unsigned int redLEDValue = 0;
unsigned int yellowLEDValue = 0;
unsigned int greenLEDValue = 0;

//LineDetector lineDetector;
cv::VideoCapture camera;
Watchdog watchdog;
UART uart_1;
GPIO gpio;

int main()
{
    //Get access to memmory shared with watchdog app
    watchdog.get_access();

    //CAMERA
    camera.open(-1);

    if (!camera.isOpened())
    {
        std::cout << "Error could not open any camera!" << std::endl
                  << "Closing app!" << std::endl;

        close_app = true;
        camera.release();
        return 0;
    }
    else
    {
        camera.set(CV_CAP_PROP_FRAME_WIDTH, 640);
        camera.set(CV_CAP_PROP_FRAME_HEIGHT, 360);
    }

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
    gpio.Export(gpio.pushButton);
    gpio.Export(gpio.redLED);
    gpio.Export(gpio.yellowLED);
    gpio.Export(gpio.greenLED);

    gpio.SetDirection(gpio.pushButton, input);
    gpio.SetDirection(gpio.redLED, output);
    gpio.SetDirection(gpio.yellowLED, output);
    gpio.SetDirection(gpio.greenLED, output);

    gpio.SetValue(gpio.redLED, low);
    gpio.SetValue(gpio.yellowLED, low);
    gpio.SetValue(gpio.greenLED, low);

    system("pause");

    //GUI
    char keypressed;

    cv::Mat frame;

    cv::namedWindow("Vision", 1);
    cv::moveWindow("Vision", 0, 500);

    //Threads
    std::thread UART_thread(U_thread, std::ref(uart_1), std::ref(gpio), std::ref(pushButtonValue), std::ref(redLEDValue), std::ref(yellowLEDValue), std::ref(greenLEDValue));

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



        push_new_data_to_UART();

        cv::imshow("Vision", frame);

        keypressed = (char)cv::waitKey(10);
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

    close_app = true;

    gpio.Unexport(gpio.pushButton);
    gpio.Unexport(gpio.redLED);
    gpio.Unexport(gpio.yellowLED);
    gpio.Unexport(gpio.greenLED);

    camera.release();

    UART_thread.join();

    watchdog.push_flag(2);
    return 0;
}

void U_thread(UART &uart, GPIO &gpio, unsigned int &pushButtonValue, unsigned int &redLEDValue, unsigned int &yellowLEDValue, unsigned int &greenLEDValue)
{
    while(!close_app)
    {
        uart.send_data();
        check_and_write_gpios(gpio, pushButtonValue, redLEDValue, yellowLEDValue, greenLEDValue);
        std::this_thread::sleep_for(std::chrono::milliseconds(30));
    }
}

void push_new_data_to_UART()
{
    uart_1.unia_danych.dane.data_0 = detected_middle_pos;
    uart_1.unia_danych.dane.data_1 = left_lane_angle_st;
    uart_1.unia_danych.dane.data_2 = right_lane_angle_st;
    //uart_1.unia_danych.dane.data_3 = 0;
    //uart_1.unia_danych.dane.data_4 = 0;
    //uart_1.unia_danych.dane.data_5 = 0;
    //uart_1.unia_danych.dane.data_6 = 0;
    //uart_1.unia_danych.dane.data_7 = 0;
    //uart_1.unia_danych.dane.flags = 0b00000000;
}

void check_and_write_gpios(GPIO &gpio, unsigned int &pushButtonValue, unsigned int &redLEDValue, unsigned int &yellowLEDValue, unsigned int &greenLEDValue)
{
    gpio.GetValue(gpio.pushButton, &pushButtonValue);
    std::cout << "       Wartosc przycisku: " << pushButtonValue << std::endl;

    gpio.SetValue(gpio.redLED, redLEDValue);
    gpio.SetValue(gpio.yellowLED, yellowLEDValue);
    gpio.SetValue(gpio.greenLED, greenLEDValue);
}
