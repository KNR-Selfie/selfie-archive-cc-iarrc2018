#include "GUI.h"

string window_name[] = {"Camera vision", "ROI", "Settings"};

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
