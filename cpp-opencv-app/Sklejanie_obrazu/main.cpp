#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <iostream>

using namespace std;
using namespace cv;

string window_name[] = {"Camera 1", "Camera 2", "Settings", "Result"};

Mat frame_1, frame_2;
Mat result_frame(1300,480, CV_8UC3, Scalar(0,0,255));

int X_pos = 600;
int width = 640, height = 480;

int main(int, char**)
{
    VideoCapture camera_1, camera_2;
    camera_1.open(1);
	camera_1.open(2);
    
    //Check if camera opened properly
    if (camera_1.isOpened() && camera_2.isOpen())
    {
        cout << "Cameras opened properly on port 1 & 2" << endl;
    }	
    else
    {
		cout << "ERROR: Could not access camera on port 1 || 2" << endl;
        return 0;
    }

    //Wait for user
    cout << "Press enter to continue ";
    getchar();

    //Initializing GUI windows
    namedWindow(0[window_name], CV_WINDOW_AUTOSIZE);
    namedWindow(1[window_name], CV_WINDOW_AUTOSIZE);
    namedWindow(2[window_name], CV_WINDOW_NORMAL);
	namedWindow(3[window_name], CV_WINDOW_AUTOSIZE);

    //Trackbar
    createTrackbar("X", window_name[2], &X_pos, 650, NULL);

    //Main function
    while(true)
    {
        camera_1 >> frame_1;
		camera_2 >> frame_2;
	}
	
	//-----
	frame_1.copyTo(result_frame(Rect(0, 0, frame_1.cols, frame_1.rows,)));
	frame_2.copyTo(result_frame(Rect(X_pos, 0, frame_1.cols, frame_1.rows,)));
	//-----
	
	imshow(0[window_name], frame_1);		
	imshow(1[window_name], frame_2);	
	imshow(3[window_name], result_frame);	
	
        if (waitKey(30) != -1)
        {
            cout << "Key pressed, closing app" << endl;
            break;
        }
    }

    camera_1.release();
	camera_2.release();
    destroyWindow(0[window_name]);
    destroyWindow(1[window_name]);
    destroyWindow(2[window_name]);
	destroyWindow(3[window_name]);
    
    return 0;
}