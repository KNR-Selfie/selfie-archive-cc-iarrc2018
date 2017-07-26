#include <iostream>
#include <cstdio>

using namespace std;

void wait_for_user();
void create_windows_trackbars(int &compared_level_main, int &ROI_vertical_pos, int &ROI_height);
void display_windows(Mat frame_1, Mat frame_2);
void display_main_ROI(Mat frame, Rect ROI_rec);
void display_detected_main_ROI_info(Mat &frame, int ROI_horizontal_pos, int ROI_vertical_pos, int ROI_width, int ROI_height, unsigned short left_top_pos, unsigned short right_top_pos, unsigned short left_bottom_pos, unsigned short right_bottom_pos, unsigned short detected_middle_top_pos, unsigned short detected_middle_bottom_pos, unsigned short detected_middle_pos);
void display_text(Mat &frame, int ROI_horizontal_pos, int ROI_vertical_pos, int ROI_width, int ROI_height, unsigned short detected_middle_pos, double bisector_angle_rad, double bisector_angle_st);