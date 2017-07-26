#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/gpu/gpu.hpp>
#include <cmath>

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

void search_to_left_top_main_ROI(Mat frame, unsigned short &left_top_pos, bool &top_left_line_visible, unsigned short ROI_width, unsigned short ROI_height, unsigned short horizontal_start_position_top);
void search_to_right_top_main_ROI(Mat frame, unsigned short &right_top_pos, bool &top_right_line_visible, unsigned short ROI_width, unsigned short ROI_height, unsigned short horizontal_start_position_top);
void search_to_left_bottom_main_ROI(Mat frame, unsigned short &left_bottom_pos, bool &bottom_left_line_visible, unsigned short ROI_width, unsigned short ROI_height, unsigned short horizontal_start_position_bottom);
void search_to_right_bottom_main_ROI(Mat frame, unsigned short &right_bottom_pos, bool &bottom_right_line_visible, unsigned short ROI_width, unsigned short ROI_height, unsigned short horizontal_start_position_bottom);
void calculate_bisector(int ROI_height, unsigned short detected_middle_bottom_pos, unsigned short detected_middle_top_pos, double &bisector_angle_rad, double &bisector_angle_st);