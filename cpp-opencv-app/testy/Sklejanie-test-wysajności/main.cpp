#include "include/camera_GPU.cpp"
#include "include/GUI.cpp"
#include "include/lane_detection.cpp"
#include "include/sign_detection.cpp"
#include "include/UART.cpp"

#define CAM_INDEX_LEFT 1
#define CAM_INDEX_RIGHT 2
#define CUDA_INDEX 0
#define CAM_WIDTH 640
#define CAM_HEIGHT 480

VideoCapture camera_left, camera_right;
int X_pos = 640;

Mat left_frame(480, 640, CV_8UC3, Scalar(200,255,200));
Mat right_frame(480, 640, CV_8UC3, Scalar(200,255,200));
Mat result_frame(480, 1280, CV_8UC3, Scalar(200,255,200));
Mat main_ROI_frame;

int compared_level = 115;

int main(int argc, char** argv)
{
    if(!start_camera(camera_left, CAM_INDEX_LEFT, CAM_WIDTH, CAM_HEIGHT))
    {
        return 0;
    }
	
	if(!start_camera(camera_right, CAM_INDEX_RIGHT, CAM_WIDTH, CAM_HEIGHT))
    {
        return 0;
    }

	if(!set_CUDA_device(CUDA_INDEX))
	{
		return 0;
	}
	/*
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
	*/
	wait_for_user();
	
	create_windows_trackbars(compared_level, ROI_horizontal_pos, ROI_height);
	
    //Main loop
    while(true)
    {
		cout << "1";
		camera_left >> left_frame;// = get_frame(camera_left);
		cout << "1";
		camera_right >> right_frame;// = get_frame(camera_right);
		
		cout << "1";
		left_frame.copyTo(result_frame(Rect(0, 0, left_frame.cols, left_frame.rows)));
		cout << "1";
		right_frame.copyTo(result_frame(Rect(X_pos, 0, left_frame.cols, left_frame.rows)));
		cout << "1";
		main_ROI_frame = filter_frame(result_frame, compared_level, ROI_horizontal_pos, ROI_vertical_pos, ROI_width, ROI_height);
		
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
		
		display_main_ROI(result_frame, ROI_rec);
		display_detected_main_ROI_info(result_frame, ROI_horizontal_pos, ROI_vertical_pos, ROI_width, ROI_height,  left_top_pos,  right_top_pos,  left_bottom_pos,  right_bottom_pos,  detected_middle_top_pos,  detected_middle_bottom_pos,  detected_middle_pos);
		display_text(result_frame, ROI_horizontal_pos, ROI_vertical_pos, ROI_width, ROI_height, detected_middle_pos, bisector_angle_rad, bisector_angle_st);
		display_windows(result_frame, main_ROI_frame);
		//GUI part
		
		if (waitKey(50) != -1)
        {
            cout << "Key pressed, closing app" << endl;
            break;
        }
    }

	destroy_handlers(camera_left, camera_right);
    return 0;
}
