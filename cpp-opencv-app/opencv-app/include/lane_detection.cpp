#include "lane_detection.h"

void search_to_left_top_main_ROI(Mat frame, unsigned short &left_top_pos, bool &top_left_line_visible, unsigned short ROI_width, unsigned short ROI_height, unsigned short horizontal_start_position_top)
{
	left_top_pos = 0;
	top_left_line_visible = false;

	for (int i = horizontal_start_position_top; i >= 0; i--)
	{
		Scalar pixel_value = frame.at<uchar>(Point(i, 1));

		if (pixel_value.val[0] == 0)
		{
			top_left_line_visible = true;
			left_top_pos = i;
			break;
		}
	}
}

void search_to_right_top_main_ROI(Mat frame, unsigned short &right_top_pos, bool &top_right_line_visible, unsigned short ROI_width, unsigned short ROI_height, unsigned short horizontal_start_position_top)
{
	right_top_pos = ROI_width - 1;
	top_right_line_visible = false;

	for (int i = horizontal_start_position_top; i < ROI_width; i++)
	{
		Scalar pixel_value = frame.at<uchar>(Point(i, 1));

		if (pixel_value.val[0] == 0)
		{
			top_right_line_visible = true;
			right_top_pos = i;
			break;
		}
	}
}

void search_to_left_bottom_main_ROI(Mat frame, unsigned short &left_bottom_pos, bool &bottom_left_line_visible, unsigned short ROI_width, unsigned short ROI_height, unsigned short horizontal_start_position_bottom)
{
	left_bottom_pos = 0;
	bottom_left_line_visible = false;

	for (int i = horizontal_start_position_bottom; i >= 0; i--)
	{
		Scalar pixel_value = frame.at<uchar>(Point(i, ROI_height - 2));

		if (pixel_value.val[0] == 0)
		{
			bottom_left_line_visible = true;
			left_bottom_pos = i;
			break;
		}
	}
}

void search_to_right_bottom_main_ROI(Mat frame, unsigned short &right_bottom_pos, bool &bottom_right_line_visible, unsigned short ROI_width, unsigned short ROI_height, unsigned short horizontal_start_position_bottom)
{
	right_bottom_pos = ROI_width - 1;
	bottom_right_line_visible = false;

	for (int i = horizontal_start_position_bottom; i < ROI_width; i++)
	{
    		Scalar pixel_value = frame.at<uchar>(Point(i, ROI_height - 2));
		
		if (pixel_value.val[0] == 0)
		{
			bottom_right_line_visible = true;
			right_bottom_pos = i;
			break;
		}
	}
}

void calculate_bisector(int ROI_height, unsigned short detected_middle_bottom_pos, unsigned short detected_middle_top_pos, double &bisector_angle_rad, double &bisector_angle_st)
{
	double a = ROI_height;
	double b = detected_middle_bottom_pos - detected_middle_top_pos;
	bisector_angle_rad = atan(a / b);

	if(bisector_angle_rad < 0)
	{
		bisector_angle_rad = 3 + bisector_angle_rad;
	}

	bisector_angle_st = bisector_angle_rad * 180 / 3.1416;
}