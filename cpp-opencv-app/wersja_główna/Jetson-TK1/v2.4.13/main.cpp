/*
 * Date: 24.10.2017
 * Author: Mateusz Grudzien
*/

#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/gpu/gpu.hpp>

#include "include/gpio_class.cpp"

#include <iostream>
#include <string>
#include <cstdio>
#include <cmath>
#include <bitset>
#include <termios.h>
#include <pthread.h>
#include <stdint.h>
#include <fcntl.h>
#include <cstdlib>
#include <stdlib.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <sys/signal.h>
#include <time.h>

#define CAM_INDEX 0
#define CUDA_INDEX 0
#define CAM_WIDTH 640
#define CAM_HEIGHT 480
#define FRAME_TIME 30

using namespace std;
using namespace cv;

//=====CAMERA VARIABLES=====

VideoCapture camera;

Mat real_frame;
Mat main_ROI_frame_filtered;

//=====CAMERA VARIABLES=====

//=====UI VARIABLES=====

string window_name[] = {"Camera vision", "ROI", "Settings"};

struct timespec start, end;
unsigned short licznik_czas = 0;

//=====UI VARIABLES=====

//=====MAIN ROI VARIABLES=====

int compared_level_main = 100;
int SZEROKOSC_LINII = 40;

int ROI_width = 620, ROI_height = 200;
int ROI_horizontal_pos = 9, ROI_vertical_pos = 150;
	
int erosion_size = 1;
Mat element = getStructuringElement(
	cv::MORPH_RECT, 
	cv::Size(2 * erosion_size, 2 * erosion_size), 
	cv::Point(erosion_size, erosion_size) );

Rect ROI_rec(
	ROI_horizontal_pos,
	ROI_vertical_pos,
	ROI_width,
	ROI_height);
	
unsigned short horizontal_start_position_top = ROI_width / 2;
unsigned short horizontal_start_position_bottom = ROI_width / 2;

unsigned short left_top_pos = 0, right_top_pos = ROI_width - 1;
unsigned short left_bottom_pos = 0, right_bottom_pos = ROI_width - 1;

bool top_left_line_visible = false;
bool bottom_left_line_visible = false;
bool top_right_line_visible = false;
bool bottom_right_line_visible = false;

unsigned short detected_middle_pos = 320;
unsigned short detected_middle_top_pos = 0;
unsigned short detected_middle_bottom_pos = ROI_width;

double bisector_angle_rad = 1.5708;
double bisector_angle_st = 90;

//=====MAIN ROI VARIABLES=====

//=====UART VARIABLES=====

struct sigaction saio;
struct termios options;

struct data_frame
{
	unsigned char sync_byte;
	unsigned short data_0 : 11;
	unsigned short data_1 : 11;
	unsigned short data_2 : 11;
	unsigned short data_3 : 11;
	unsigned short data_4 : 11;
	unsigned short data_5 : 11;
	unsigned short data_6 : 11;
	unsigned short data_7 : 11;
	unsigned char flags;
	unsigned char end_byte;
} __attribute__((packed));

union frame
{
	data_frame dane;
	char data_to_send[14];
};

frame unia_danych;

int fd = 0;
bool check = false;


//=====UART VARIABLES=====

//=====NEW=====

bool czy_reset = false;

int sukces = 1;
unsigned int buttonValue = low;

GPIO gpio;

int pushButton = 163;
int redLED = 164;
int yellowLED = 165;
int greenLED = 166;

int poziom_do_wysylki = 0;
bool strona = 0;

Scalar srednia_srodek;
bool czy_stop = false;
int odleglosc_do_stop = 0;
bool czy_sprawdzac_stop = 1;

int skrzyzowanie_pos_x = 115;
int skrzyzowanie_pos_y = 80;
int skrzyzowanie_width_poprawka = 250;
int skrzyzowanie_height = 120;

bool czy_zgubiono_trase = false;
bool czy_wypadlismy_calkowicie = false;
bool z_ktorej_strony_trasa = 0;
unsigned short szerokosc_trasy = 300;

//=====NEW=====

//=====FUNCTION DECLARATIONS=====

bool start_camera(int camera_index, unsigned short camera_width, unsigned short camera_height);
void destroy_handlers();
bool set_CUDA_device(unsigned short CUDA_device_index);
void filter_main_ROI_frame();

void wait_for_user();
void create_windows_trackbars();
void display_main_ROI();
void display_detected_main_ROI_info();
void display_text();

void search_to_left_top_main_ROI();
void search_to_right_top_main_ROI();
void search_to_left_bottom_main_ROI();
void search_to_right_bottom_main_ROI();
void calculate_bisector();

bool UART_set();
void send_UART_data();

void reset_if_cannot_see();
void reset();
void stop();

//=====FUNCTION DECLARATIONS=====

//=====MAIN=====

int main(int argc, char** argv)
{
    if(!start_camera(CAM_INDEX, CAM_WIDTH, CAM_HEIGHT))
    {
      return 0;
    }

    if(!set_CUDA_device(CUDA_INDEX))
    {
      return 0;
    }

    UART_set();

      unia_danych.dane.sync_byte = 0xff;
      unia_danych.dane.flags = 0b00000000;
      unia_danych.dane.end_byte = 0xfe;

    
    //=====NEW=====
    gpio.gpioExport(pushButton);
    gpio.gpioSetDirection(pushButton,input);
    gpio.gpioExport(redLED);
    gpio.gpioSetDirection(redLED,output);
    gpio.gpioExport(yellowLED);
    gpio.gpioSetDirection(yellowLED,output);
    gpio.gpioExport(greenLED);
    gpio.gpioSetDirection(greenLED,output);
    //=====NEW=====

    //wait_for_user();

    create_windows_trackbars();	

    moveWindow(0[window_name], 0, 500);
    moveWindow(1[window_name], 700, 500 + ROI_vertical_pos);
    moveWindow(2[window_name], 800, 0);
    
    setNumThreads(4);

    //Main loop
    while(true)
    {
        // Start time
        if(licznik_czas == 0)
        {
            clock_gettime(CLOCK_MONOTONIC, &start);
        }

        cout << "\033[2J\033[1;1H";

        camera >> real_frame;
        filter_main_ROI_frame();

        sukces = gpio.gpioGetValue(pushButton, &buttonValue);

        if(!sukces)
        {
            cout << "          Wartosc odczytana z pinu 49: " << buttonValue << endl;

            if(buttonValue)
            {
                reset();
                cout << "          Zresetowano wizje!" << endl;
                czy_reset = true;
            }
            else
            {
                czy_reset = false;
            }
        }
        else
        {
            cout << "          Problemy z odczytem z pinu 49!" << endl;
        }

        //reset_if_cannot_see();
/*
        if(czy_zgubiono_trase)
        {
            if(z_ktorej_strony_trasa)
            {
                detected_middle_bottom_pos = ROI_width - 5;
                detected_middle_top_pos = ROI_width - 5;
            }
            else
            {
                detected_middle_bottom_pos = 5;
                detected_middle_top_pos = 5;
            }
        }
*/
        if(!czy_reset)
        {
            if(czy_wypadlismy_calkowicie)
            {
                if(z_ktorej_strony_trasa)
                {
                        horizontal_start_position_bottom = ROI_width - 5;
			horizontal_start_position_top = ROI_width - 5;                
                }
                else
                {
		  horizontal_start_position_bottom = 5;                           
                  horizontal_start_position_top = 5;   
                }
            }
        }

        search_to_left_top_main_ROI();
        search_to_right_top_main_ROI();
        search_to_left_bottom_main_ROI();
        search_to_right_bottom_main_ROI();

	if(!czy_wypadlismy_calkowicie)
	{
	  horizontal_start_position_top = min(left_top_pos, right_top_pos) + abs(right_top_pos - left_top_pos) / 2;
	  horizontal_start_position_bottom = min(left_bottom_pos, right_bottom_pos) + abs(right_bottom_pos - left_bottom_pos) / 2;;	
	}
	
        detected_middle_top_pos = ROI_horizontal_pos + min(left_top_pos, right_top_pos) + abs(left_top_pos - right_top_pos) / 2;
        detected_middle_bottom_pos = ROI_horizontal_pos + min(left_bottom_pos, right_bottom_pos) + abs(left_bottom_pos - right_bottom_pos) / 2;
        detected_middle_pos = ROI_horizontal_pos + min(detected_middle_top_pos, detected_middle_bottom_pos) + abs(detected_middle_top_pos - detected_middle_bottom_pos) / 2;

        if(!czy_zgubiono_trase)
        {
            calculate_bisector();
        }

        if(detected_middle_pos > 320)
        {
            poziom_do_wysylki = detected_middle_pos - 320;
            strona = 1;

            if(poziom_do_wysylki > 254)
            {
                poziom_do_wysylki = 254;
            }
        }
        else
        {
            poziom_do_wysylki = abs(320 - detected_middle_pos);
            strona = 0;

            if(poziom_do_wysylki > 254)
            {
                poziom_do_wysylki = 254;
            }
        }
/*
        if(czy_sprawdzac_stop)
        {
            stop();
        }
*/
        if(!czy_zgubiono_trase)
        {
            if(bottom_left_line_visible && bottom_right_line_visible && top_left_line_visible && top_right_line_visible)
            {
                szerokosc_trasy = ((right_bottom_pos - left_bottom_pos) + (right_top_pos - left_top_pos))/2;
            }

            if(!top_left_line_visible || !bottom_left_line_visible)
            {
                czy_zgubiono_trase = true;
                z_ktorej_strony_trasa = 0;
                czy_sprawdzac_stop = false;

                goto idz_do;
            }

            if(!top_right_line_visible || !bottom_right_line_visible)
            {
                czy_zgubiono_trase = true;
                z_ktorej_strony_trasa = 1;
                czy_sprawdzac_stop = false;

                goto idz_do;
            }
        }
        else
        {
            if(bottom_left_line_visible && bottom_right_line_visible && top_left_line_visible && top_right_line_visible)
            {
                unsigned short szerokosc_tmp = right_bottom_pos - left_bottom_pos;

                if(szerokosc_tmp > SZEROKOSC_LINII)
                {
                    czy_zgubiono_trase = false;
                    czy_wypadlismy_calkowicie = false;
                    czy_sprawdzac_stop = true;
                }
            }

            idz_do: //goto przy pierwszej klatce poza trasa

            if(z_ktorej_strony_trasa)
            {
                if(bottom_left_line_visible && top_left_line_visible)
                {
                    poziom_do_wysylki = (left_bottom_pos + left_top_pos) / 2 + szerokosc_trasy / 2 - 320;
                    strona = 1;
                    czy_wypadlismy_calkowicie = false;
                }
                else
                {
                    poziom_do_wysylki = 400;
                    strona = 1;
                    czy_wypadlismy_calkowicie = true;
                }
            }
            else
            {
                if(bottom_right_line_visible && top_right_line_visible)
                {
                    poziom_do_wysylki = abs(320 - (right_bottom_pos + right_top_pos) / 2  + szerokosc_trasy / 2);
                    strona = 0;
                    czy_wypadlismy_calkowicie = false;
                }
                else
                {
                    poziom_do_wysylki = 400;
                    strona = 0;
                    czy_wypadlismy_calkowicie = true;
                }
            }
        }

        //UART part
        unia_danych.dane.data_0 = strona;
        unia_danych.dane.data_1 = poziom_do_wysylki;
        unia_danych.dane.data_2 = bisector_angle_st;
        unia_danych.dane.data_3 = odleglosc_do_stop;
        unia_danych.dane.data_4 = 0b00011111111;
        unia_danych.dane.data_5 = 0b00111111111;
        unia_danych.dane.data_6 = 0b01111111111;
        unia_danych.dane.data_7 = 0b11111111111;
        unia_danych.dane.flags = czy_stop;

        send_UART_data();
        //UART part

        //UI part

        cout << "          Strona: " << strona << endl;
        cout << "          Odchylka: " << poziom_do_wysylki << endl;
        cout << "          Kat: " << bisector_angle_st << endl;
        cout << "          STOP: " << czy_stop << endl;
        cout << "          Odleglosc do stop: " << odleglosc_do_stop << endl;
        cout << "          Szerokosc: " << szerokosc_trasy << endl;
        cout << "          Czy wypadlimy: " << czy_zgubiono_trase << endl;
        cout << "          Czy wypadlimy calkowicie: " << czy_wypadlismy_calkowicie << endl;
        cout << "          Line visibility: " << top_left_line_visible << " " << top_right_line_visible << endl;
        cout << "                           " << bottom_left_line_visible << " " << bottom_right_line_visible << endl;
	
	cout << endl << "Ilosc procesow: " << getNumThreads() << endl << endl;
	cout << endl << "Numer procesu:  " << getThreadNum() << endl << endl;

        display_main_ROI();
        display_detected_main_ROI_info();
        display_text();

        imshow(0[window_name], real_frame);                 //Camera vision
        imshow(1[window_name], main_ROI_frame_filtered);	//main ROI threshold
        //UI part

        // Time elapsed
        double miliseconds;
        if(licznik_czas > 500)
        {
            licznik_czas = 0;
            clock_gettime(CLOCK_MONOTONIC, &end);
            miliseconds = (end.tv_sec - start.tv_sec);
        }
        else
        {
            licznik_czas++;
        }

        cout << "          Time in seconds [500 frames]: " << miliseconds << endl;

        // Calculate frames per second
        double fps  =  1 / (miliseconds / 500);
        cout << "          Estimated frames per second : " << fps << endl;

        char wyjscie = waitKey(FRAME_TIME);

        switch(wyjscie)
        {
            case 'r':
            {
                reset();
                cout << "          Reset" << endl;
                czy_reset = true;
                break;
            }

            case 'p':
            {
                if(czy_sprawdzac_stop)
                {
                    czy_sprawdzac_stop = false;
                    czy_stop = true;
                }
                else
                {
                    czy_sprawdzac_stop = true;
                    czy_stop = false;
                }
                break;
            }

            case 27:
            {
                cout << "          Key pressed, closing app" << endl;

                destroy_handlers();
                return 0;
            }
        }
    }

    destroy_handlers();
    return 0;
}

//=====MAIN=====

//=====CAMERA FUNCTIONS=====

bool start_camera(int camera_index, unsigned short camera_width, unsigned short camera_height)
{
    camera.open(camera_index);
    
    if (camera.isOpened())
    {
        cout << "Camera opened properly on port 0" << endl;
        cout << "Resolution: " << camera.get(CV_CAP_PROP_FRAME_WIDTH) << " x " << camera.get(CV_CAP_PROP_FRAME_HEIGHT) << endl;
        return 1;
    }	
    else
    {
        cout << "ERROR: Could not access camera on port 0" << endl;		
        return 0;
    }
}

void destroy_handlers()
{
    gpio.gpioUnexport(pushButton);
    gpio.gpioUnexport(redLED);
    gpio.gpioUnexport(yellowLED);
    gpio.gpioUnexport(greenLED);

    camera.release();

    gpu::resetDevice();
}

bool set_CUDA_device(unsigned short CUDA_device_index)
{
    //Check of apropriate GPU module with CUDA enabled
    if(!gpu::getCudaEnabledDeviceCount())
    {
        cout << "No CUDA module detected!" << endl;
        cout << "Closing app!" << endl;
        return 0;
    }
    else
    {
        cout << "Detected " << gpu::getCudaEnabledDeviceCount() << " CUDA device/s" << endl;
    }
    
    //Setting of GPU module on adress 0
    gpu::setDevice(CUDA_device_index);

    //Check of proper initializing of CUDA device on adress 0
    gpu::DeviceInfo CUDA_device;
    if(CUDA_device.deviceID() == 0 && CUDA_device.isCompatible() == true)
    {
        cout << "CUDA device initialized properly on adress 0" << endl;
    }
    else
    {
        cout << "Initializing CUDA device on adress 0 failed!" << endl;
        cout << "Closing app!" << endl;
        return 0;
    }

    //Display info about CUDA device
    cout << "CUDA device name: " << CUDA_device.name() << endl;
    cout << "Version: " << CUDA_device.majorVersion() << "." << CUDA_device.minorVersion() << endl;
    cout << "Number of streaming multiprocessors: " << CUDA_device.multiProcessorCount() << endl;
    cout << "Total device GDDR5 memory: " << CUDA_device.totalMemory() << " bytes" << endl;
    cout << "Amount of free GDDR5 memory: " << CUDA_device.freeMemory() << " bytes" << endl;
    return 1;
}

void filter_main_ROI_frame()
{
	Mat frame_ROI, frame_from_gpu;
	gpu::GpuMat gpu_frame, gpu_hsv, gpu_range_frame, gpu_filtered;
	vector<gpu::GpuMat> gpu_hsv_split;
	real_frame(ROI_rec).copyTo(frame_ROI);

	gpu_frame.upload(frame_ROI);

	gpu::cvtColor(gpu_frame, gpu_hsv, CV_BGR2HSV);
	gpu::split(gpu_hsv, gpu_hsv_split);
        gpu::threshold(gpu_hsv_split[2], gpu_range_frame, compared_level_main, 255, THRESH_BINARY);
	dilate(gpu_range_frame, gpu_filtered, element);

	gpu_filtered.download(main_ROI_frame_filtered);
}

//=====CAMERA FUNCTIONS=====

//=====UI FUNCTIONS=====

void wait_for_user()
{
    cout << "Press enter to continue ";
    getchar();
}

void create_windows_trackbars()
{
	//Initializing GUI windows
    namedWindow(2[window_name], CV_WINDOW_NORMAL);
    namedWindow(1[window_name], CV_WINDOW_AUTOSIZE);  
    namedWindow(0[window_name], CV_WINDOW_AUTOSIZE);

    //Trackbar to set level of threshold of main search
    createTrackbar("Threshold", window_name[2], &compared_level_main, 255, NULL);
    //Trackbar to set ROI height
    createTrackbar("ROI height", window_name[2], &ROI_height, 200, NULL);
    //Trackbar to set ROI vertical position
    createTrackbar("ROI ver", window_name[2], &ROI_vertical_pos, 400, NULL);
    //Trackbar to set
    createTrackbar("Srod x", window_name[2], &skrzyzowanie_pos_x, 255, NULL);
    //Trackbar to set
    createTrackbar("Srod y", window_name[2], &skrzyzowanie_pos_y, 255, NULL);
    //Trackbar to set
    createTrackbar("Srod wi", window_name[2], &skrzyzowanie_width_poprawka, 255, NULL);
    //Trackbar to set
    createTrackbar("Srod hi", window_name[2], &skrzyzowanie_height, 255, NULL);
}

void display_main_ROI()
{
	rectangle(
		real_frame, 
		ROI_rec,
		Scalar(0, 255, 0), 
		1, 
		8, 
		0);
}

void display_detected_main_ROI_info()
{
	//Left top line detection
	line(
		real_frame,
		Point(ROI_horizontal_pos + left_top_pos, ROI_vertical_pos + 10),
		Point(ROI_horizontal_pos + left_top_pos, ROI_vertical_pos - 10),
        Scalar(0, 0, 255
               ),
		2,
		8,
		0);

	//Right top line detection
	line(
		real_frame,
		Point(ROI_horizontal_pos + right_top_pos, ROI_vertical_pos + 10),
		Point(ROI_horizontal_pos + right_top_pos, ROI_vertical_pos - 10),
        Scalar(0, 0, 255),
		2,
		8,
		0);

	//Left bottom line detection
	line(
		real_frame,
		Point(ROI_horizontal_pos + left_bottom_pos, ROI_vertical_pos + ROI_height + 10),
		Point(ROI_horizontal_pos + left_bottom_pos, ROI_vertical_pos + ROI_height - 10),
        Scalar(0, 0, 255),
		2,
		8,
		0);

	//Right bottom line detection
	line(
		real_frame,
		Point(ROI_horizontal_pos + right_bottom_pos, ROI_vertical_pos + ROI_height + 10),
		Point(ROI_horizontal_pos + right_bottom_pos, ROI_vertical_pos + ROI_height - 10),
        Scalar(0, 0, 255),
		2,
		8,
		0);

	//Bisector visual representation
	line(
		real_frame,
		Point(ROI_horizontal_pos + detected_middle_top_pos, ROI_vertical_pos),
		Point(ROI_horizontal_pos + detected_middle_bottom_pos, ROI_vertical_pos + ROI_height),
		Scalar(100, 100, 0),
		2,
		8,
		0);

	//Detected middle representation
	line(
		real_frame,
		Point(detected_middle_pos, ROI_vertical_pos + ROI_height / 2 + 20),
		Point(detected_middle_pos, ROI_vertical_pos + ROI_height / 2 - 20),
        Scalar(0, 100, 200),
		2,
		8,
		0);
}

void display_text()
{
	string s_1, s_2, s_3;
	stringstream out_1, out_2, out_3;

	out_1 << "Kat [rad]: " << bisector_angle_rad;
	s_1 = out_1.str();
	out_2 << "Kat [st]:  " << bisector_angle_st;
	s_2 = out_2.str();
	out_3 << "Srodek:  " << detected_middle_pos;
	s_3 = out_3.str();

	putText(real_frame, s_1, Point(310, 30), CV_FONT_HERSHEY_COMPLEX, 1, Scalar(255, 96, 0), 2, 8);
	putText(real_frame, s_2, Point(310, 70), CV_FONT_HERSHEY_COMPLEX, 1, Scalar(0, 96, 255), 2, 8);
	putText(real_frame, s_3, Point(310, 110), CV_FONT_HERSHEY_COMPLEX, 1, Scalar(0, 255, 0), 2, 8);

	putText(
		real_frame, 
		"ROI", 
		Point(ROI_horizontal_pos, ROI_vertical_pos - 15),
		FONT_HERSHEY_SIMPLEX,
		1,
		Scalar(0, 100, 200),
		2,
		8,
		0);
}


//=====UI FUNCTIONS=====

//=====MAIN ROI FUNCTIONS=====

void search_to_left_top_main_ROI()
{
	left_top_pos = 0;
	top_left_line_visible = false;

	for (int i = horizontal_start_position_top; i > 0; i--)
	{
		Scalar pixel_value = main_ROI_frame_filtered.at<uchar>(Point(i, 1));

        if (pixel_value.val[0] == 255)
		{
			top_left_line_visible = true;
			left_top_pos = i;
			break;
		}
	}
}

void search_to_right_top_main_ROI()
{
	right_top_pos = ROI_width - 1;
	top_right_line_visible = false;

	for (int i = horizontal_start_position_top; i < ROI_width-1; i++)
	{
		Scalar pixel_value = main_ROI_frame_filtered.at<uchar>(Point(i, 1));

        if (pixel_value.val[0] == 255)
		{
			top_right_line_visible = true;
			right_top_pos = i;
			break;
		}
	}
}

void search_to_left_bottom_main_ROI()
{
	left_bottom_pos = 0;
	bottom_left_line_visible = false;

	for (int i = horizontal_start_position_bottom; i > 0; i--)
	{
		Scalar pixel_value = main_ROI_frame_filtered.at<uchar>(Point(i, ROI_height - 2));

        if (pixel_value.val[0] == 255)
		{
			bottom_left_line_visible = true;
			left_bottom_pos = i;
			break;
		}
	}
}

void search_to_right_bottom_main_ROI()
{
	right_bottom_pos = ROI_width - 1;
	bottom_right_line_visible = false;

	for (int i = horizontal_start_position_bottom; i < ROI_width -1; i++)
	{
    		Scalar pixel_value = main_ROI_frame_filtered.at<uchar>(Point(i, ROI_height - 2));
		
        if (pixel_value.val[0] == 255)
		{
			bottom_right_line_visible = true;
			right_bottom_pos = i;
			break;
		}
	}
}

void calculate_bisector()
{
    if(detected_middle_bottom_pos == detected_middle_top_pos)
    {
        bisector_angle_rad = 1.5708;
        bisector_angle_st = 90;
    }
    else
    {
        double a = ROI_height;
        double b = abs(detected_middle_bottom_pos - detected_middle_top_pos);

        if(detected_middle_bottom_pos < detected_middle_top_pos)
        {
            bisector_angle_rad = 3.1416 - atan(a / b);
        }
        else
        {
            bisector_angle_rad = atan(a / b);
        }
    }

    bisector_angle_st = bisector_angle_rad * 57.295;
}

//=====MAIN ROI FUNCTIONS=====

//=====UART FUNCTIONS=====

bool UART_set()
{
    fd = open("/dev/ttyTHS1", O_RDWR | O_NOCTTY | O_NDELAY);
    if (fd == -1)
    {
        cout << "\033[1;31m Opening Serial port failed\033[0m\n" << endl;
        return 0;
    }
    else
    {
        cout << "\033[1;32m Opening Serial port succeded\033[0m\n" << endl;
    }

    cout << "\033[1;33m Setting Serial options\033[0m\n" << endl;

    //Setting UART options
    //Get the current options for the port
    tcgetattr(fd, &options);

    //Set the baud rates to 115200
    cfsetispeed(&options, B115200);
    cfsetospeed(&options, B115200);
	
    //Enable parity
    options.c_cflag |= PARENB;
    //Set parity to even
    options.c_cflag &= ~PARODD;
	
    //2 stop bits
    options.c_cflag |= CSTOPB;
	
    //Mask the character size bits and set 8 data bits
    options.c_cflag &= ~CSIZE;
    options.c_cflag |= CS8;

   //Set the new options for the port...
    tcsetattr(fd, TCSANOW, &options);

    //Clear UART buffer
    tcflush(fd, TCIOFLUSH);	
	
    cout << "\033[1;32m Serial options set \033[0m\n" << endl;
    return 1;
}

void send_UART_data()
{
    for(int i = 0; i < 14; i++)
    {
        //cout << (int)unia_danych.data_to_send[i] << endl;
        check = write(fd, &unia_danych.data_to_send[i], 1);
    }
}

//=====UART FUNCTIONS=====

//=====NEW=====

void reset_if_cannot_see()
{
    if(!bottom_left_line_visible && !bottom_right_line_visible)
    {
        horizontal_start_position_bottom = ROI_width/2;
    }

    if(!top_left_line_visible && !top_right_line_visible)
    {
        horizontal_start_position_top = ROI_width/2;
    }
/*
    if(right_bottom_pos - left_bottom_pos < 100)
    {
        horizontal_start_position_bottom = ROI_width/2;
    }

    if(right_top_pos - left_top_pos < 100)
    {
        horizontal_start_position_top = ROI_width/2;
    }
*/
}

void reset()
{
    horizontal_start_position_bottom = ROI_width/2;
    horizontal_start_position_top = ROI_width/2;
    czy_wypadlismy_calkowicie = false;
    czy_zgubiono_trase = false;
}

void stop()
{
    if(bottom_left_line_visible && bottom_right_line_visible && right_bottom_pos - left_bottom_pos - skrzyzowanie_width_poprawka > 2)
    {
        Rect skrzyzowanie_srodek_rec(
                    left_bottom_pos + skrzyzowanie_pos_x,
                    skrzyzowanie_pos_y,
                    right_bottom_pos - left_bottom_pos - skrzyzowanie_width_poprawka,
                    skrzyzowanie_height);

        Rect skrzyzowanie_srodek_draw_rec(
                    left_bottom_pos + skrzyzowanie_pos_x + ROI_horizontal_pos,
                    skrzyzowanie_pos_y + ROI_vertical_pos,
                    right_bottom_pos - left_bottom_pos - skrzyzowanie_width_poprawka,
                    skrzyzowanie_height);

        Mat skrzyzowanie_ROI_srodek;
        main_ROI_frame_filtered(skrzyzowanie_srodek_rec).copyTo(skrzyzowanie_ROI_srodek);

        srednia_srodek = mean(skrzyzowanie_ROI_srodek);

        rectangle(
            real_frame,
            skrzyzowanie_srodek_draw_rec,
            Scalar(255, 0, 0),
            2,
            8,
            0);

        string s, s_1;
        stringstream out, out_1;

        out << "Srednia srodek:  " << srednia_srodek;
        s = out.str();

        putText(real_frame, s, Point(50, 400), CV_FONT_HERSHEY_COMPLEX, 1, Scalar(255, 0, 255), 2, 8);

        if(srednia_srodek[0] > 15)
        {
            out_1 << "STOP";
            s_1 = out_1.str();

            czy_stop = true;

            putText(real_frame, s_1, Point(150, 450), CV_FONT_HERSHEY_COMPLEX, 1, Scalar(255, 255, 255), 2, 8);
        }
        else
        {
            czy_stop = false;
        }
    }
    else
    {
        cout << "Brak odpowiedniej ilosci obrazu w kadrze!" << endl;
    }

    if(czy_stop)
    {
        for (int i = 1; i < ROI_height - 2; i++)
        {
            Scalar pixel_value = main_ROI_frame_filtered.at<uchar>(Point(detected_middle_pos, i));

            if (pixel_value.val[0] == 255)
            {
                odleglosc_do_stop = ROI_height - i;
                break;
            }
        }
    }
    else
    {
        odleglosc_do_stop = 0;
    }
}

//=====NEW=====
