#include "include/camera_GPU.cpp"
#include "include/GUI.cpp"
#include "include/lane_detection.cpp"
#include "include/sign_detection.cpp"
#include "include/UART.cpp"

#define CAM_INDEX 0
#define CUDA_INDEX 0
#define CAM_WIDTH 640
#define CAM_HEIGHT 480

//=====CAMERA VARIABLES=====

VideoCapture camera;

Mat real_frame;
Mat main_ROI_frame;

//=====CAMERA VARIABLES=====

//=====GUI VARIABLES=====

string window_name[] = {"Camera vision", "ROI", "Settings"};

//=====GUI VARIABLES=====

//=====MAIN ROI VARIABLES=====

int compared_level = 115;

int ROI_width = 620, ROI_height = 100;
int ROI_horizontal_pos = 9, ROI_vertical_pos = 200;
	
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
Mat get_frame(unsigned short camera_index);
Mat filter_frame(Mat frame, unsigned short compared_level_main, unsigned short ROI_horizontal_pos, unsigned short ROI_vertical_pos, unsigned short ROI_width, unsigned short ROI_height);

void wait_for_user();
void create_windows_trackbars(int &compared_level_main, int &ROI_vertical_pos, int &ROI_height);
void display_windows(Mat frame_1, Mat frame_2);
void display_main_ROI(Mat frame, Rect ROI_rec);
void display_detected_main_ROI_info(Mat &frame, int ROI_horizontal_pos, int ROI_vertical_pos, int ROI_width, int ROI_height, unsigned short left_top_pos, unsigned short right_top_pos, unsigned short left_bottom_pos, unsigned short right_bottom_pos, unsigned short detected_middle_top_pos, unsigned short detected_middle_bottom_pos, unsigned short detected_middle_pos);
void display_text(Mat &frame, int ROI_horizontal_pos, int ROI_vertical_pos, int ROI_width, int ROI_height, unsigned short detected_middle_pos, double bisector_angle_rad, double bisector_angle_st);

void search_to_left_top_main_ROI(Mat frame, unsigned short &left_top_pos, bool &top_left_line_visible, unsigned short ROI_width, unsigned short ROI_height, unsigned short horizontal_start_position_top);
void search_to_right_top_main_ROI(Mat frame, unsigned short &right_top_pos, bool &top_right_line_visible, unsigned short ROI_width, unsigned short ROI_height, unsigned short horizontal_start_position_top);
void search_to_left_bottom_main_ROI(Mat frame, unsigned short &left_bottom_pos, bool &bottom_left_line_visible, unsigned short ROI_width, unsigned short ROI_height, unsigned short horizontal_start_position_bottom);
void search_to_right_bottom_main_ROI(Mat frame, unsigned short &right_bottom_pos, bool &bottom_right_line_visible, unsigned short ROI_width, unsigned short ROI_height, unsigned short horizontal_start_position_bottom);
void calculate_bisector(int ROI_height, unsigned short detected_middle_bottom_pos, unsigned short detected_middle_top_pos, double &bisector_angle_rad, double &bisector_angle_st);

bool UART_set();
void send_UART_data();

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
	
	wait_for_user();
	
	create_windows_trackbars(compared_level, ROI_horizontal_pos, ROI_height);
	
    //Main loop
    while(true)
    {
		real_frame = get_frame(CAM_INDEX);
		main_ROI_frame = filter_frame(real_frame, compared_level, ROI_horizontal_pos, ROI_vertical_pos, ROI_width, ROI_height);
		
		search_to_left_top_main_ROI(main_ROI_frame, left_top_pos, top_left_line_visible, ROI_width, ROI_height, horizontal_start_position_top);
		search_to_right_top_main_ROI(main_ROI_frame, right_top_pos, top_right_line_visible, ROI_width, ROI_height, horizontal_start_position_top);
		search_to_left_bottom_main_ROI(main_ROI_frame, left_bottom_pos, bottom_left_line_visible, ROI_width, ROI_height, horizontal_start_position_bottom);
		search_to_right_bottom_main_ROI(main_ROI_frame, right_bottom_pos, bottom_right_line_visible, ROI_width, ROI_height, horizontal_start_position_bottom);
		
		horizontal_start_position_top = min(left_top_pos, right_top_pos) + abs(right_top_pos - left_top_pos) / 2;
		horizontal_start_position_bottom = min(left_bottom_pos, right_bottom_pos) + abs(right_bottom_pos - left_bottom_pos) / 2;;	
		
		detected_middle_top_pos = ROI_horizontal_pos + min(left_top_pos, right_top_pos) + abs(left_top_pos - right_top_pos) / 2;
		detected_middle_bottom_pos = ROI_horizontal_pos + min(left_bottom_pos, right_bottom_pos) + abs(left_bottom_pos - right_bottom_pos) / 2;
		detected_middle_pos = ROI_horizontal_pos + min(detected_middle_top_pos, detected_middle_bottom_pos) + abs(detected_middle_top_pos - detected_middle_bottom_pos) / 2;
		
		calculate_bisector(ROI_height, detected_middle_bottom_pos, detected_middle_top_pos, bisector_angle_rad, bisector_angle_st);
		
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
		cout << "Middle: " << detected_middle_pos << endl;
		
		Rect ROI_rec(
		ROI_horizontal_pos,
		ROI_vertical_pos,
		ROI_width,
		ROI_height);
		
		display_main_ROI(real_frame, ROI_rec);
		display_detected_main_ROI_info(real_frame, ROI_horizontal_pos, ROI_vertical_pos, ROI_width, ROI_height,  left_top_pos,  right_top_pos,  left_bottom_pos,  right_bottom_pos,  detected_middle_top_pos,  detected_middle_bottom_pos,  detected_middle_pos);
		display_text(real_frame, ROI_horizontal_pos, ROI_vertical_pos, ROI_width, ROI_height, detected_middle_pos, bisector_angle_rad, bisector_angle_st);
		display_windows(real_frame, main_ROI_frame);
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

Mat get_frame(unsigned short camera_index)
{
        Mat frame;	

        camera >> frame;
	
	if(!frame.data)
	{
		cout << "No image data!" << endl;
	}
	
	return frame;
}

Mat filter_frame(Mat frame, unsigned short compared_level_main, unsigned short ROI_horizontal_pos, unsigned short ROI_vertical_pos, unsigned short ROI_width, unsigned short ROI_height)
{
	Mat frame_ROI, frame_from_gpu;
	gpu::GpuMat gpu_frame, gpu_hsv, gpu_range_frame, gpu_filtered;
	vector<gpu::GpuMat> gpu_hsv_split;
	
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

	frame(ROI_rec).copyTo(frame_ROI);

	gpu_frame.upload(frame_ROI);

	gpu::cvtColor(gpu_frame, gpu_hsv, CV_BGR2HSV);
	gpu::split(gpu_hsv, gpu_hsv_split);
	gpu::threshold(gpu_hsv_split[2], gpu_range_frame, compared_level_main, 255, THRESH_BINARY);
	dilate(gpu_range_frame, gpu_filtered, element);

	gpu_filtered.download(frame_from_gpu);
	
	return frame_from_gpu;
}

//=====CAMERA FUNCTIONS=====

//=====GUI FUNCTIONS=====

void wait_for_user()
{
    cout << "Press enter to continue ";
    getchar();
}

void create_windows_trackbars(int &compared_level_main, int &ROI_vertical_pos, int &ROI_height)
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

}

void display_windows(Mat frame_1, Mat frame_2)
{
	imshow(0[window_name], frame_1);		//Camera vision 
	imshow(1[window_name], frame_2);		//main ROI threshold
}

void display_main_ROI(Mat frame, Rect ROI_rec)
{
	rectangle(
		frame, 
		ROI_rec,
		Scalar(0, 255, 0), 
		1, 
		8, 
		0);
}

void display_detected_main_ROI_info(Mat &frame, int ROI_horizontal_pos, int ROI_vertical_pos, int ROI_width, int ROI_height, unsigned short left_top_pos, unsigned short right_top_pos, unsigned short left_bottom_pos, unsigned short right_bottom_pos, unsigned short detected_middle_top_pos, unsigned short detected_middle_bottom_pos, unsigned short detected_middle_pos)
{
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
}

void display_text(Mat &frame, int ROI_horizontal_pos, int ROI_vertical_pos, int ROI_width, int ROI_height, unsigned short detected_middle_pos, double bisector_angle_rad, double bisector_angle_st)
{
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
}


//=====GUI FUNCTIONS=====

//=====MAIN ROI FUNCTIONS=====

void search_to_left_top_main_ROI(Mat frame, unsigned short &left_top_pos, bool &top_left_line_visible, unsigned short ROI_width, unsigned short ROI_height, unsigned short horizontal_start_position_top)
{
	left_top_pos = 0;
	top_left_line_visible = false;

	for (int i = horizontal_start_position_top; i >= 0; i--)
	{
		Scalar pixel_value = frame.at<uchar>(Point(i, 1));

		if (pixel_value.val[0] == 0)
		{
			top_left_line_visible = true;
			left_top_pos = i;
			break;
		}
	}
}

void search_to_right_top_main_ROI(Mat frame, unsigned short &right_top_pos, bool &top_right_line_visible, unsigned short ROI_width, unsigned short ROI_height, unsigned short horizontal_start_position_top)
{
	right_top_pos = ROI_width - 1;
	top_right_line_visible = false;

	for (int i = horizontal_start_position_top; i < ROI_width; i++)
	{
		Scalar pixel_value = frame.at<uchar>(Point(i, 1));

		if (pixel_value.val[0] == 0)
		{
			top_right_line_visible = true;
			right_top_pos = i;
			break;
		}
	}
}

void search_to_left_bottom_main_ROI(Mat frame, unsigned short &left_bottom_pos, bool &bottom_left_line_visible, unsigned short ROI_width, unsigned short ROI_height, unsigned short horizontal_start_position_bottom)
{
	left_bottom_pos = 0;
	bottom_left_line_visible = false;

	for (int i = horizontal_start_position_bottom; i >= 0; i--)
	{
		Scalar pixel_value = frame.at<uchar>(Point(i, ROI_height - 2));

		if (pixel_value.val[0] == 0)
		{
			bottom_left_line_visible = true;
			left_bottom_pos = i;
			break;
		}
	}
}

void search_to_right_bottom_main_ROI(Mat frame, unsigned short &right_bottom_pos, bool &bottom_right_line_visible, unsigned short ROI_width, unsigned short ROI_height, unsigned short horizontal_start_position_bottom)
{
	right_bottom_pos = ROI_width - 1;
	bottom_right_line_visible = false;

	for (int i = horizontal_start_position_bottom; i < ROI_width; i++)
	{
    		Scalar pixel_value = frame.at<uchar>(Point(i, ROI_height - 2));
		
		if (pixel_value.val[0] == 0)
		{
			bottom_right_line_visible = true;
			right_bottom_pos = i;
			break;
		}
	}
}

void calculate_bisector(int ROI_height, unsigned short detected_middle_bottom_pos, unsigned short detected_middle_top_pos, double &bisector_angle_rad, double &bisector_angle_st)
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
