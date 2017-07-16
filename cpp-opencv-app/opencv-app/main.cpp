#include "include/camera_GPU.cpp"
#include "include/GUI.cpp"
#include "include/lane_detection.cpp"
#include "include/sign_detection.cpp"
#include "include/UART.cpp"

#define CAM_INDEX 0
#define CUDA_INDEX 0
#define CAM_WIDTH 640
#define CAM_HEIGHT 480

Mat real_frame;
Mat main_ROI_frame;

unsigned short compared_level_main = 115;
unsigned short ROI_width = 620, ROI_height = 100;
unsigned short ROI_horizontal_pos = 9, ROI_vertical_pos = 200;

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
	
	wait_for_user();
	
    //Main loop
    while(true)
    {
		real_frame = get_frame(CAM_INDEX);
		main_ROI_frame = filter_frame(real_frame, compared_level_main, ROI_horizontal_pos, ROI_vertical_pos, ROI_width, ROI_height);
		
		display_windows(real_frame, main_ROI_frame);
		
		if (waitKey(50) != -1)
        {
            cout << "Key pressed, closing app" << endl;
            break;
        }
    }

	destroy_handlers(CAM_INDEX);
    return 0;
}
