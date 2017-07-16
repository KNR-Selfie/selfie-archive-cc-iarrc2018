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
