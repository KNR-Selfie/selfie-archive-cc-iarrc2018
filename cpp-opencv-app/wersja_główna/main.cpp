#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/gpu/gpu.hpp>

#include "include/GPIO.cpp"

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

#define CAM_INDEX 0
#define CUDA_INDEX 0
#define CAM_WIDTH 640
#define CAM_HEIGHT 480

using namespace std;
using namespace cv;

//=====NEW=====

jetsonGPIO yellowLED = gpio165 ;

bool czy_na_skrzyzowaniu = false;
bool ostatnio_pixel_skrzyzowania = false;

//=====NEW=====

//=====CAMERA VARIABLES=====

VideoCapture camera;

Mat real_frame;
Mat main_ROI_frame_filtered;

//=====CAMERA VARIABLES=====

//=====GUI VARIABLES=====

string window_name[] = {"Camera vision", "ROI", "Settings"};

//=====GUI VARIABLES=====

//=====MAIN ROI VARIABLES=====

int compared_level_main = 115;
int SZEROKOSC_LINII = 40;

int ROI_width = 620, ROI_height = 100;
int ROI_horizontal_pos = 9, ROI_vertical_pos = 200;
	
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

double bisector_angle_rad = 0.5;
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

void czy_jest_skrzyzowanie();
void wypisz_o_skrzyzowaniu();

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
    
    if(!UART_set())
    {
      return 0;
    }
    else
    {
      unia_danych.dane.sync_byte = 0xff;
      unia_danych.dane.flags = 0b00000000;
      unia_danych.dane.end_byte = 0xff;
    }
    
    //=====NEW=====

    gpioExport(yellowLED);
    gpioSetDirection(yellowLED,outputPin);

    //=====NEW=====

    wait_for_user();

    create_windows_trackbars();	
    
    //Main loop
    while(true)
    {
        camera >> real_frame;
        filter_main_ROI_frame();

        search_to_left_top_main_ROI();
        search_to_right_top_main_ROI();
        search_to_left_bottom_main_ROI();
        search_to_right_bottom_main_ROI();
        
        horizontal_start_position_top = min(left_top_pos, right_top_pos) + abs(right_top_pos - left_top_pos) / 2;
        horizontal_start_position_bottom = min(left_bottom_pos, right_bottom_pos) + abs(right_bottom_pos - left_bottom_pos) / 2;;	

        detected_middle_top_pos = ROI_horizontal_pos + min(left_top_pos, right_top_pos) + abs(left_top_pos - right_top_pos) / 2;
        detected_middle_bottom_pos = ROI_horizontal_pos + min(left_bottom_pos, right_bottom_pos) + abs(left_bottom_pos - right_bottom_pos) / 2;
        detected_middle_pos = ROI_horizontal_pos + min(detected_middle_top_pos, detected_middle_bottom_pos) + abs(detected_middle_top_pos - detected_middle_bottom_pos) / 2;

        calculate_bisector();
        
        czy_jest_skrzyzowanie();
        
          
  cout << "czy_skrzyzowanie: " << czy_na_skrzyzowaniu << endl;
  cout << "ostatnio: " << ostatnio_pixel_skrzyzowania << endl;
        
        if(czy_na_skrzyzowaniu)
        {
          gpioSetValue(yellowLED, high);
          wypisz_o_skrzyzowaniu();
        }
        else
        {
          gpioSetValue(yellowLED, low);
        }
        
        //UART part
        unia_danych.dane.data_0 = 0b11111111111;
        unia_danych.dane.data_1 = 0b11111111111;
        unia_danych.dane.data_2 = 0b11111111111;
        unia_danych.dane.data_3 = 0b11111111111;
        unia_danych.dane.data_4 = 0b11111111111;
        unia_danych.dane.data_5 = 0b11111111111;
        unia_danych.dane.data_6 = 0b11111111111;
        unia_danych.dane.data_7 = 0b11111111111;
        unia_danych.dane.flags = 0xff;

        send_UART_data();
        //UART part

        //GUI part

        display_main_ROI();
        display_detected_main_ROI_info();
        display_text();
        
        imshow(0[window_name], real_frame);		//Camera vision 
        imshow(1[window_name], main_ROI_frame_filtered);	//main ROI threshold
        //GUI part

        if (waitKey(50) != -1)
        {
            cout << "Key pressed, closing app" << endl;
            break;
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
	gpu::threshold(gpu_hsv_split[2], gpu_range_frame, compared_level_main, 255, THRESH_BINARY_INV);
	dilate(gpu_range_frame, gpu_filtered, element);

	gpu_filtered.download(main_ROI_frame_filtered);
}

//=====CAMERA FUNCTIONS=====

//=====GUI FUNCTIONS=====

void wait_for_user()
{
    cout << "Press enter to continue ";
    getchar();
}

void create_windows_trackbars()
{
	//Initializing GUI windows
    namedWindow(0[window_name], CV_WINDOW_AUTOSIZE);
    namedWindow(1[window_name], CV_WINDOW_AUTOSIZE);
    namedWindow(2[window_name], CV_WINDOW_NORMAL);

    //Trackbar to set level of threshold of main search
    createTrackbar("Threshold", window_name[2], &compared_level_main, 255, NULL);
    //Trackbar to set ROI height
    createTrackbar("ROI height", window_name[2], &ROI_height, 200, NULL);
    //Trackbar to set ROI vertical position
    createTrackbar("ROI ver", window_name[2], &ROI_vertical_pos, 400, NULL);
    //Trackbar to set level of threshold of main search
    createTrackbar("Line wid", window_name[2], &SZEROKOSC_LINII, 100, NULL);

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
		Scalar(255, 0, 0),
		2,
		8,
		0);

	//Right top line detection
	line(
		real_frame,
		Point(ROI_horizontal_pos + right_top_pos, ROI_vertical_pos + 10),
		Point(ROI_horizontal_pos + right_top_pos, ROI_vertical_pos - 10),
		Scalar(255, 0, 0),
		2,
		8,
		0);

	//Left bottom line detection
	line(
		real_frame,
		Point(ROI_horizontal_pos + left_bottom_pos, ROI_vertical_pos + ROI_height + 10),
		Point(ROI_horizontal_pos + left_bottom_pos, ROI_vertical_pos + ROI_height - 10),
		Scalar(255, 0, 0),
		2,
		8,
		0);

	//Right bottom line detection
	line(
		real_frame,
		Point(ROI_horizontal_pos + right_bottom_pos, ROI_vertical_pos + ROI_height + 10),
		Point(ROI_horizontal_pos + right_bottom_pos, ROI_vertical_pos + ROI_height - 10),
		Scalar(255, 0, 0),
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


//=====GUI FUNCTIONS=====

//=====MAIN ROI FUNCTIONS=====

void search_to_left_top_main_ROI()
{
	left_top_pos = 0;
	top_left_line_visible = false;

	for (int i = horizontal_start_position_top; i > 0; i--)
	{
		Scalar pixel_value = main_ROI_frame_filtered.at<uchar>(Point(i, 1));

		if (pixel_value.val[0] == 0)
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

		if (pixel_value.val[0] == 0)
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

		if (pixel_value.val[0] == 0)
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
		
		if (pixel_value.val[0] == 0)
		{
			bottom_right_line_visible = true;
			right_bottom_pos = i;
			break;
		}
	}
}

void calculate_bisector()
{
	double a = ROI_height;
	double b = detected_middle_bottom_pos - detected_middle_top_pos;
	bisector_angle_rad = atan(a / b);

	if(bisector_angle_rad < 0)
	{
		bisector_angle_rad = 3 + bisector_angle_rad;
	}

	bisector_angle_st = bisector_angle_rad * 180 / 3.1416;
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

void czy_jest_skrzyzowanie()
{
  if(bottom_right_line_visible && right_bottom_pos + SZEROKOSC_LINII < ROI_width)
  {
    Scalar pixel_value = main_ROI_frame_filtered.at<uchar>(Point(right_bottom_pos + SZEROKOSC_LINII, ROI_height - 10));
    
    //cout << pixel_value.val[0] << endl << endl << endl;
    
    if (pixel_value.val[0] == 0)
    {
      if(!ostatnio_pixel_skrzyzowania)
      {
        if(czy_na_skrzyzowaniu)
        {
          czy_na_skrzyzowaniu = false;
        }
        else
        {
          czy_na_skrzyzowaniu = true;
        }
      }
      
      ostatnio_pixel_skrzyzowania = true;
    }
    else
    {
      ostatnio_pixel_skrzyzowania = false;
    }
  }
  else
  {
    cout << "Nie widac odpowiedniej ilosci drogi w kadrze" << endl;
  }
}

void wypisz_o_skrzyzowaniu()
{
  string s;
  stringstream out;
  out << "SKRZYZOWANIE";
  s = out.str();

  putText(real_frame, s, Point(200, 180), CV_FONT_HERSHEY_COMPLEX, 1, Scalar(0, 0, 255), 2, 8);
}

//=====NEW=====
