#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/gpu/gpu.hpp>

void search_to_left_top_main_ROI(Mat frame, unsigned short &left_top_pos, bool &top_left_line_visible, unsigned short ROI_width, unsigned short ROI_height, unsigned short horizontal_start_position_top);
void search_to_right_top_main_ROI(Mat frame, unsigned short &right_top_pos, bool &top_right_line_visible, unsigned short ROI_width, unsigned short ROI_height, unsigned short horizontal_start_position_top);
void search_to_left_bottom_main_ROI(Mat frame, unsigned short &left_bottom_pos, bool &bottom_left_line_visible, unsigned short ROI_width, unsigned short ROI_height, unsigned short horizontal_start_position_bottom);
void search_to_right_bottom_main_ROI(Mat frame, unsigned short &right_bottom_pos, bool &bottom_right_line_visible, unsigned short ROI_width, unsigned short ROI_height, unsigned short horizontal_start_position_bottom);