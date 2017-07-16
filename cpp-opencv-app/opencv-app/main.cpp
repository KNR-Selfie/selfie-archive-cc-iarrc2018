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
	
	create_windows_trackbars(compared_level, ROI_horizontal_pos, ROI_height);
	
	wait_for_user();
	
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
		
		//GUI part
		cout << "Middle: " << detected_middle_pos << endl;
		
		Rect ROI_rec(
		ROI_horizontal_pos,
		ROI_vertical_pos,
		ROI_width,
		ROI_height);
		
		display_main_ROI(real_frame, ROI_rec);
		display_detected_main_ROI_info(real_frame, ROI_horizontal_pos, ROI_vertical_pos, ROI_width, ROI_height,  left_top_pos,  right_top_pos,  left_bottom_pos,  right_bottom_pos,  detected_middle_top_pos,  detected_middle_bottom_pos,  detected_middle_pos);
		void display_text(real_frame, ROI_horizontal_pos, ROI_vertical_pos, ROI_width, ROI_height, detected_middle_pos, bisector_angle_rad, bisector_angle_st);
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
