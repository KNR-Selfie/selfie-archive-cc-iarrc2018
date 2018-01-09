#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/gpu/gpu.hpp>
#include <iostream>
#include <stdio.h>
#include <cmath>
#include <termios.h>
#include <pthread.h>
#include <stdint.h>
#include <fcntl.h>
#include <cstdlib>
#include <stdlib.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <sys/signal.h>

using namespace std;
using namespace cv;

//UART structures 
struct sigaction saio;
struct termios options;

void clear_data(int state, void* param);
void start_working(int state, void* param);
void stop_working(int state, void* param);
void UART_set();
void UART_receive();
void UART_send();
void UART_decide();
void make_data_to_send();
void* search_to_left_top_ROI();
void* search_to_right_top_ROI();
void* search_to_left_bottom_ROI();
void* search_to_right_bottom_ROI();
void search_stop_line_left();
void search_stop_line_right();
void calculate_bisector();

//OpenCV variables
string window_name[] = {"Camera vision", "ROI", "Settings"};

Mat frame, frame_from_gpu, frame_ROI;
gpu::GpuMat gpu_frame, gpu_hsv, gpu_range_frame, gpu_filtered, gpu_brg, gpu_gray;

vector<gpu::GpuMat> gpu_hsv_split;

//Logic variables
int compared_level_main = 115;
int ROI_width = 620, ROI_height = 100;
int ROI_horizontal_pos = 9, ROI_vertical_pos = 200;

int horizontal_start_position_top = ROI_width / 2;
int horizontal_start_position_bottom = ROI_width / 2;

int left_top_pos = 0, right_top_pos = ROI_width - 1;
int left_bottom_pos = 0, right_bottom_pos = ROI_width - 1;

bool top_left_line_visible = false;
bool bottom_left_line_visible = false;
bool top_right_line_visible = false;
bool bottom_right_line_visible = false;

bool working = false;

int detected_middle_pos = 320;
int detected_middle_top_pos = 0;
int detected_middle_bottom_pos = ROI_width;

int stop_pos_left = -1;
int stop_pos_right = -1;
bool left_stop_line_visible = false;
bool right_stop_line_visible = false;

double bisector_angle_rad = 0.5;
double bisector_angle_st = 90;



// Create a structuring element for erosion and dilation
int erosion_size = 1;  
Mat element = getStructuringElement(
	cv::MORPH_RECT, 
	cv::Size(2 * erosion_size, 2 * erosion_size), 
	cv::Point(erosion_size, erosion_size) );

//UART variables
bool new_UART_received = false;
int fd = 0, check = 0, bytes_read = 0;
char UART_data_received[7] = {0, 0, 0, 0, 0, 0, 11};
char UART_data_to_send[7] = {0, 0, 0, 0, 0, 0, 11};
char read_buffer[64]; 
 
int main(int, char**)
{
    //Set UART comunication
    UART_set();

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
    gpu::setDevice(0);

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

    //Initializing of camera device on port 0
    VideoCapture camera;
    camera.open(1);
    
    //Check if camera opened properly
    if (camera.isOpened())
    {
        cout << "Camera opened properly on port 0" << endl;
	cout << "Resolution: " << camera.get(CV_CAP_PROP_FRAME_WIDTH) << " x " << camera.get(CV_CAP_PROP_FRAME_HEIGHT) << endl;
    }	
    else
    {
	cout << "ERROR: Could not access camera on port 0" << endl;
        return 0;
    }

    //Wait for user
    cout << "Press enter to continue ";
    getchar();

    //Initializing GUI windows
    namedWindow(0[window_name], CV_WINDOW_AUTOSIZE);
    namedWindow(1[window_name], CV_WINDOW_AUTOSIZE);
    namedWindow(2[window_name], CV_WINDOW_NORMAL);

	//Button to reset start search position to the middle of frame
	cvCreateButton("START", start_working, NULL, CV_PUSH_BUTTON, 0);
	//Button to reset start search position to the middle of frame
	cvCreateButton("STOP", stop_working, NULL, CV_PUSH_BUTTON, 0);
    //Button to reset start search position to the middle of frame
    cvCreateButton("RESET", clear_data, NULL, CV_PUSH_BUTTON, 0);

    //Trackbar to set level of threshold of main search
    createTrackbar("Threshold", window_name[2], &compared_level_main, 255, NULL);
    //Trackbar to set ROI height
    createTrackbar("ROI height", window_name[2], &ROI_height, 200, NULL);
    //Trackbar to set ROI vertical position
    createTrackbar("ROI ver", window_name[2], &ROI_vertical_pos, 400, NULL);

    //Main function
    while(true)
    {
		if (!working)
		{
			UART_data_received = { 0, 0, 0, 0, 0, 0, 11 };
			UART_data_to_send = { 100, 0, 0, 0, 0, 0, 11 };

			while (UART_data_received[0] != 100)
			{
				UART_send();
				UART_receive();
				usleep(500000);
			}

			working = true;
		}

        camera >> frame;

	Rect ROI_rec(
		ROI_horizontal_pos,
		ROI_vertical_pos,
		ROI_width,
		ROI_height);

	frame(ROI_rec).copyTo(frame_ROI);

	gpu_frame.upload(frame_ROI);

	gpu::cvtColor(gpu_frame, gpu_hsv, CV_BGR2HSV);
	gpu::split(gpu_hsv, gpu_hsv_split);
	gpu::threshold(gpu_hsv_split[2], gpu_range_frame, compared_level_main, 255, THRESH_BINARY);
	dilate(gpu_range_frame, gpu_filtered, element);

	gpu_filtered.download(frame_from_gpu);

	search_to_left_top_ROI();
	search_to_right_top_ROI();
	search_to_left_bottom_ROI();
	search_to_right_bottom_ROI();

	horizontal_start_position_top = min(left_top_pos, right_top_pos) + abs(right_top_pos - left_top_pos) / 2;
	horizontal_start_position_bottom = min(left_bottom_pos, right_bottom_pos) + abs(right_bottom_pos - left_bottom_pos) / 2;;	

	detected_middle_top_pos = ROI_horizontal_pos + min(left_top_pos, right_top_pos) + abs(left_top_pos - right_top_pos) / 2;
	detected_middle_bottom_pos = ROI_horizontal_pos + min(left_bottom_pos, right_bottom_pos) + abs(left_bottom_pos - right_bottom_pos) / 2;
	detected_middle_pos = ROI_horizontal_pos + min(detected_middle_top_pos, detected_middle_bottom_pos) + abs(detected_middle_top_pos - detected_middle_bottom_pos) / 2;

	search_stop_line_left();
	search_stop_line_right();

	if (left_stop_line_visible && right_stop_line_visible)
	{
		out_1 << "STOP";
		s_1 = out_1.str();

		putText(frame, s_1, Point(200, 60), CV_FONT_HERSHEY_COMPLEX, 1, Scalar(0, 0, 255), 2, 8);

		line(
			frame,
			Point(max(left_bottom_pos, left_top_pos), stop_pos_left),
			Point(min(right_bottom_pos, right_top_pos), stop_pos_right),
			Scalar(0, 0, 255),
			3,
			8,
			0);
	}

	calculate_bisector();

	//UART part
	UART_receive();
    UART_decide();
	make_data_to_send();
	UART_send();

	//Terminal part
	cout << detected_middle_pos << endl;
	cout << "LT: " << left_top_pos << ", ";	
	cout << "RT: " << right_top_pos << ", ";	
	cout << "LB: " << left_bottom_pos << ", ";	
	cout << "RB: " << right_bottom_pos << endl;

	cout << "Detected lines: " << top_left_line_visible << " , " << top_right_line_visible << endl;
	cout << "                " << bottom_left_line_visible << " , " << bottom_right_line_visible << endl;

	//GUI part
	string s_1, s_2, s_3;
	stringstream out_1, out_2, out_3;

	out_1 << "Kat [rad]: " << bisector_angle_rad;
	s_1 = out_1.str();
	out_2 << "Kat [st]:  " << bisector_angle_st;
	s_2 = out_2.str();
	out_3 << "Srodek:  " << detected_middle_pos;
	s_3 = out_3.str();

	putText(frame, s_1, Point(310, 30), CV_FONT_HERSHEY_COMPLEX, 1, Scalar(255, 96, 0), 2, 8);
	putText(frame, s_2, Point(310, 70), CV_FONT_HERSHEY_COMPLEX, 1, Scalar(0, 96, 255), 2, 8);
	putText(frame, s_3, Point(310, 110), CV_FONT_HERSHEY_COMPLEX, 1, Scalar(0, 255, 0), 2, 8);

	rectangle(
		frame, 
		ROI_rec,
		Scalar(0, 255, 0), 
		1, 
		8, 
		0);

	//Left top line detection
	line(
		frame,
		Point(ROI_horizontal_pos + left_top_pos, ROI_vertical_pos + 10),
		Point(ROI_horizontal_pos + left_top_pos, ROI_vertical_pos - 10),
		Scalar(255, 0, 0),
		2,
		8,
		0);

	//Right top line detection
	line(
		frame,
		Point(ROI_horizontal_pos + right_top_pos, ROI_vertical_pos + 10),
		Point(ROI_horizontal_pos + right_top_pos, ROI_vertical_pos - 10),
		Scalar(255, 0, 0),
		2,
		8,
		0);

	//Left bottom line detection
	line(
		frame,
		Point(ROI_horizontal_pos + left_bottom_pos, ROI_vertical_pos + ROI_height + 10),
		Point(ROI_horizontal_pos + left_bottom_pos, ROI_vertical_pos + ROI_height - 10),
		Scalar(255, 0, 0),
		2,
		8,
		0);

	//Right bottom line detection
	line(
		frame,
		Point(ROI_horizontal_pos + right_bottom_pos, ROI_vertical_pos + ROI_height + 10),
		Point(ROI_horizontal_pos + right_bottom_pos, ROI_vertical_pos + ROI_height - 10),
		Scalar(255, 0, 0),
		2,
		8,
		0);

	//Bisector visual representation
	line(
		frame,
		Point(ROI_horizontal_pos + detected_middle_top_pos, ROI_vertical_pos),
		Point(ROI_horizontal_pos + detected_middle_bottom_pos, ROI_vertical_pos + ROI_height),
		Scalar(100, 100, 0),
		2,
		8,
		0);

	//Detected middle representation
	line(
		frame,
		Point(detected_middle_pos, ROI_vertical_pos + ROI_height / 2 + 20),
		Point(detected_middle_pos, ROI_vertical_pos + ROI_height / 2 - 20),
		Scalar(0, 100, 200),
		2,
		8,
		0);

	putText(
		frame, 
		"ROI", 
		Point(ROI_horizontal_pos, ROI_vertical_pos - 15),
		FONT_HERSHEY_SIMPLEX,
		1,
		Scalar(0, 100, 200),
		2,
		8,
		0);

	imshow(0[window_name], frame);			//Camera vision 
	imshow(1[window_name], frame_from_gpu);		//ROI threshold 

        if (waitKey(50) != -1)
        {
            cout << "Key pressed, closing app" << endl;
            break;
        }
    }

    //Destroying used handles and GUI windows
    camera.release();
    gpu::resetDevice();
    destroyWindow(0[window_name]);
    destroyWindow(1[window_name]);
    destroyWindow(2[window_name]);
    
    return 0;
}

//----------------------------------------------------

void clear_data(int state, void* param)
{
	horizontal_start_position_top = ROI_width / 2;
	horizontal_start_position_bottom = ROI_width / 2;
}

void start_working(int state, void* param)
{
	working = true;
}

void stop_working(int state, void* param)
{
	working = false;
}

void UART_set()
{
    //Open port for UART
    fd = open("/dev/ttyTHS1", O_RDWR | O_NOCTTY | O_NDELAY);  //albo /ttyTHS1
    if (fd == -1)
    {
  	cout << "\033[1;31m Opening Serial port failed\033[0m\n" << endl;
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

    //Enable the receiver and set local mode
    options.c_cflag |= (CLOCAL | CREAD);
	
    //No parity
    options.c_cflag &= ~PARENB;
	
    //1 stop bit
    options.c_cflag &= ~CSTOPB;
	
    //Mask the character size bits and set 8 data bits
    options.c_cflag &= ~CSIZE;
    options.c_cflag |= CS8;

    //Disable hardware flow control
    options.c_cflag &= ~CRTSCTS;

    //Choosing raw input
    options.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG);

    //
    options.c_cflag |= (CLOCAL | CREAD);

    //Disabling software flow control
    options.c_iflag &= ~(IXON | IXOFF | IXANY);

    //
    options.c_oflag &= ~OPOST;

   //Set the new options for the port...
    tcsetattr(fd, TCSANOW, &options);

    //Clear UART buffer
    tcflush(fd, TCIOFLUSH);	
	
    cout << "\033[1;32m Serial options set \033[0m\n" << endl;
}

void UART_receive()
{
    bytes_read = read(fd, &read_buffer, 7); 

    if(bytes_read != 7)
    {
	cout << "\033[1;31m No data! \033[0m\n" << endl;
    }
    else
    {
	if(read_buffer[6] != 11)
	{
	    cout << "\033[1;31m Control byte incorrect! \033[0m\n" << endl;
	}
	else
	{
	    cout << "\033[1;32m Control byte correct! \033[0m\n" << endl;
	    for(int i = 0; i < 7; i++)
	    {
	    	UART_data_received[i] = read_buffer[i];
	    }

	    new_UART_received = true;
	}
    }

    //Clear UART buffer
    tcflush(fd, TCIOFLUSH);
}

void UART_send()
{
    for(int i = 0; i < 7; i++)
    {
		check = write(fd, &UART_data_to_send, 7);
    }
}

void UART_decide()
{
    if(new_UART_received)
    {
		//Funkcje decydujące czy coś robić
		switch (UART_data_received[0])
		{
			case 100:
			{
				working = true;
				break;
			}

			case 101:
			{
				working = false;
				break;
			}

			case 200:
			{

				break;
			}
		}

		new_UART_received = false;
    }
    else
    {
		//Nic nie otrzymano - jakiś licznik do sprawdzenia czy nie rozłączono
    }
}

void make_data_to_send()
{
    //Convert error and bisector angle to UART vector
}

void* search_to_left_top_ROI()
{
	left_top_pos = 0;
	top_left_line_visible = false;

	for (int i = horizontal_start_position_top; i >= 0; i--)
	{
		Scalar pixel_value = frame_from_gpu.at<uchar>(Point(i, 1));

		if (pixel_value.val[0] == 0)
		{
			top_left_line_visible = true;
			left_top_pos = i;
			break;
		}
	}
}

void* search_to_right_top_ROI()
{
	right_top_pos = ROI_width - 1;
	top_right_line_visible = false;

	for (int i = horizontal_start_position_top; i < ROI_width; i++)
	{
		Scalar pixel_value = frame_from_gpu.at<uchar>(Point(i, 1));

		if (pixel_value.val[0] == 0)
		{
			top_right_line_visible = true;
			right_top_pos = i;
			break;
		}
	}
}

void* search_to_left_bottom_ROI()
{
	left_bottom_pos = 0;
	bottom_left_line_visible = false;

	for (int i = horizontal_start_position_bottom; i >= 0; i--)
	{
		Scalar pixel_value = frame_from_gpu.at<uchar>(Point(i, ROI_height - 2));

		if (pixel_value.val[0] == 0)
		{
			bottom_left_line_visible = true;
			left_bottom_pos = i;
			break;
		}
	}
}

void* search_to_right_bottom_ROI()
{
	right_bottom_pos = ROI_width - 1;
	bottom_right_line_visible = false;

	for (int i = horizontal_start_position_bottom; i < ROI_width; i++)
	{
    		Scalar pixel_value = frame_from_gpu.at<uchar>(Point(i, ROI_height - 2));
		
		if (pixel_value.val[0] == 0)
		{
			bottom_right_line_visible = true;
			right_bottom_pos = i;
			break;
		}
	}
}

void search_stop_line_left()
{
	left_stop_line_visible = false;

	for (int i = ROI_height - 1; i < 1; i--)
	{
		Scalar pixel_value = frame_from_gpu.at<uchar>(Point(max(left_bottom_pos, left_top_pos), i));

		if (pixel_value.val[0] == 0)
		{
			left_stop_line_visible = true;
			stop_pos_left = i;
			break;
		}
	}
}

void search_stop_line_right()
{
	right_stop_line_visible = false;

	for (int i = ROI_height - 1; i < 1; i--)
	{
		Scalar pixel_value = frame_from_gpu.at<uchar>(Point(min(right_bottom_pos, right_top_pos), i));

		if (pixel_value.val[0] == 0)
		{
			right_stop_line_visible = true;
			stop_pos_right = i;
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

/*
	Dodać przycisk do zerowania ustawienia środka
	Sprawdzić czy wielowątkowość coś wgl da
	Dodać UART w nowym standardzie
*/

// ******************* READ the Pixel intensity *********************
// single channel grey scale image (type 8UC1) and pixel coordinates x=5 and y=2
// by convention, {row number = y} and {column number = x}
// intensity.val[0] contains a value from 0 to 255, 0 = black, 255 = white

/*
{
	double a = dolny_rzad - gorny_rzad;
	double b = lewe_polozenie_dol - lewe_polozenie_gora;
	double kat_rad = atan(a / b);
	double kat_st = kat_rad * 180 / 3.1416;
}
*/
