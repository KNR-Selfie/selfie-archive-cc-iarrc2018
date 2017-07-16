#include "camera.h"

bool start_camera(unsigned short camera_index, unsigned short camera_width, unsigned short camera_height)
{
    //Initializing of camera device on port 0
    camera.open(camera_index);
    
    //Check if camera opened properly
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
