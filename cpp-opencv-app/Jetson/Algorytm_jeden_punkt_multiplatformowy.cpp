/*
* Test algorytmu pod¹¿ania za lini¹ z komunikacj¹ z p³ytk¹ STM32
*
* Created: 28.03.2017
* Author : Mateusz Grudzieñ
*/

#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <iostream>

#ifdef _WIN64

#include <windows.h>
#include <process.h>

#elif __linux__ 

#include <pthread.h>

#endif

#define SZEROKOSC 640
#define WYSOKOSC 480
#define POROWNYWANY_ODCIEN 60

using namespace std;
using namespace cv;

Mat frame, hsv_img;
vector<Mat> hsv_split;
Vec3b value_vector;

short pixel_value[2] = { 128, 128 };
volatile int left_position, right_position;
Point left_point, right_point, gorny_punkt, dolny_punkt;

int err = 0;

#ifdef _WIN64

void scan_left(void * dummy);
void scan_right(void * dummy);

#elif __linux__

void *scan_left(void * dummy);
void *scan_right(void * dummy);

int x = 0;
pthread_t tid_1;
pthread_t tid_2;
#endif

int main()
{
	VideoCapture camera;
	camera.open(0);
	camera.set(CV_CAP_PROP_FRAME_WIDTH, SZEROKOSC);
	camera.set(CV_CAP_PROP_FRAME_HEIGHT, WYSOKOSC);

	if (!camera.isOpened())
	{
		cout << "ERROR: Could not access camera on port 0" << endl;
		return 0;
	}

	namedWindow("Obraz z kamery", CV_WINDOW_AUTOSIZE);

	while (true)
	{
		camera >> frame;
		cvtColor(frame, hsv_img, CV_BGR2HSV);

		left_position = -1;
		right_position = -1;

#ifdef _WIN64

		HANDLE Thread_1 = (HANDLE)_beginthread(scan_left, 0, NULL);
		HANDLE Thread_2 = (HANDLE)_beginthread(scan_right, 0, NULL);

		WaitForSingleObject(Thread_1, INFINITE);
		WaitForSingleObject(Thread_2, INFINITE);

#elif __linux__ 


		pthread_create(&tid_1, NULL, scan_left, &x);
		pthread_create(&tid_2, NULL, scan_right, &x);

		pthread_join(tid_1, NULL);
		pthread_join(tid_2, NULL);
#endif

		if (left_position != -1 && right_position != -1)
		{
			if (right_position - left_position > 0 && right_position - left_position < 200)
			{
				left_point = Point(left_position, WYSOKOSC / 2);
				right_point = Point(right_position, WYSOKOSC / 2);
				gorny_punkt = Point((right_position - left_position) / 2 + left_position, WYSOKOSC / 2 + 30);
				dolny_punkt = Point((right_position - left_position) / 2 + left_position, WYSOKOSC / 2 - 30);

				line(frame, left_point, right_point, (20, 100, 0), 3, 8, 0);
				line(frame, gorny_punkt, dolny_punkt, (20, 100, 0), 3, 8, 0);
			}
		}

#ifdef _WIN64
		if (left_position != -1 && right_position != -1)
		{
			err = left_position + (right_position - left_position) / 2 - SZEROKOSC / 2;
			cout << err << endl;
		}

#elif __linux__

		if (left_position != -1 && right_position != -1)
		{
			err = left_position + (right_position - left_position) / 2 - SZEROKOSC / 2;
		}

		//UART communication
#endif

		imshow("Obraz z kamery", frame);

		if (waitKey(35) == 27)
		{
			cout << "ESC key pressed" << endl;
			break;
		}
	}

	camera.release();
	destroyWindow("Obraz z kamery");

	return 0;
}

#ifdef _WIN64

void scan_left(void * dummy)
{
	for (int i = 0; i < SZEROKOSC; i++)
	{
		value_vector = hsv_img.at<Vec3b>(WYSOKOSC / 2, i);
		0 [pixel_value] = value_vector.val[2];

		if (0 [pixel_value] < POROWNYWANY_ODCIEN)
		{
			if (1 [pixel_value] < POROWNYWANY_ODCIEN)
			{
				left_position = i;
				break;
			}
		}
		1 [pixel_value] = 0 [pixel_value];
	}

	_endthread();
}

void scan_right(void * dummy)
{
	for (int i = SZEROKOSC - 1; i >= 0; i--)
	{
		value_vector = hsv_img.at<Vec3b>(WYSOKOSC / 2, i);
		0 [pixel_value] = value_vector.val[2];

		if (0 [pixel_value] < POROWNYWANY_ODCIEN)
		{
			if (1 [pixel_value] < POROWNYWANY_ODCIEN)
			{
				right_position = i;
				break;
			}
		}
		1 [pixel_value] = 0 [pixel_value];
	}

	_endthread();
}

#elif __linux__

void *scan_left(void * dummy)
{
	for (int i = 0; i < SZEROKOSC; i++)
	{
		value_vector = hsv_img.at<Vec3b>(WYSOKOSC / 2, i);
		0 [pixel_value] = value_vector.val[2];

		if (0 [pixel_value] < POROWNYWANY_ODCIEN)
		{
			if (1 [pixel_value] < POROWNYWANY_ODCIEN)
			{
				left_position = i;
				break;
			}
		}
		1 [pixel_value] = 0 [pixel_value];
	}
}

void *scan_right(void * dummy)
{
	for (int i = SZEROKOSC - 1; i >= 0; i--)
	{
		value_vector = hsv_img.at<Vec3b>(WYSOKOSC / 2, i);
		0 [pixel_value] = value_vector.val[2];

		if (0 [pixel_value] < POROWNYWANY_ODCIEN)
		{
			if (1 [pixel_value] < POROWNYWANY_ODCIEN)
			{
				right_position = i;
				break;
			}
		}
		1 [pixel_value] = 0 [pixel_value];
	}
}
#endif