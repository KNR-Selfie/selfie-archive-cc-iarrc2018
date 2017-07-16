#include "GUI.h"

void wait_for_user()
{
    cout << "Press enter to continue ";
    getchar();
}

void display_windows(Mat frame_1, Mat frame_2)
{
	imshow(0[window_name], frame_1);		//Camera vision 
	imshow(1[window_name], frame_2);		//main ROI threshold
}