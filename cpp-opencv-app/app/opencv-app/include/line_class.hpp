#pragma once

#include "opencv2/opencv.hpp"
#include <opencv2/highgui/highgui.hpp>
#include<iostream>
#include <vector>
#include <string>

class LineDetector
{
	//Struktura do przechowywania wszstkich wykrytych punktow
    struct Punkt
    {
        cv::Point coordinates;
        float slope;
    };

	//Wszystkie punkty w parach wykrytych linii
	std::vector<cv::Vec4i> lines;
	
	//Wszystkie punkty wraz z ich nachyleniami
    std::vector<Punkt> all_points;

	//Wszystkie punkty nalezace do pionowych linii podzielone na 4 sektory
	std::vector<Punkt> BL_points;
	std::vector<Punkt> BR_points;
	std::vector<Punkt> TL_points;
	std::vector<Punkt> TR_points;

	//Wszystkie punkty nalezace do poziomych linii
	std::vector<Punkt> horizontal_points;
	std::vector<Punkt> accepted_horizontal_points;

	//Zapisane polozenie i nachylenie srodka z poprzedniej klatki
	Punkt last_top_middle_point;
	Punkt last_bottom_middle_point;

	//Flagi bool
	bool 	TL_sector,
			BL_sector,
			TR_sector,
			BR_sector,
			Horizontal_lines;


	bool	Detect_parking_line;	
	bool	Vertical_line;

	float 	new_slope_left; 
	float 	new_slope_right; 
	int 	new_pos_left; 
	int 	new_pos_right; 
	int 	new_middle;

	int left_ang_st;
	int right_ang_st;
	
	// 0 - left lane, 1 - right lane
	bool current_lane;
	int offset_counter_1;
	int offset_counter_2;
	int emergency_counter;

public:
	int width;
	bool	Cross_line;
	bool 	Parking_line;
	LineDetector()
	{
		last_top_middle_point.coordinates = cv::Point(320,150);
		last_bottom_middle_point.coordinates = cv::Point(320,300);
		
		last_top_middle_point.slope = 999;
		last_bottom_middle_point.slope = 999;

		new_slope_left = 999; 
		new_slope_right = 999; 
		new_pos_left = 70; 
		new_pos_right = 570; 
	 	new_middle = 320;
	 	width = 500;

		current_lane = 1;
		offset_counter_1 = 0;
		offset_counter_2 = 0;
		emergency_counter = 0;
	}

//OFFSET

	bool first_step = false;
	bool second_step = false;

//OFFSET

	//Dane do wysylki
	int srodek = 320;
	float lewy_kat = 90;
	float prawy_kat = 90;

	//
    void applyBlur(cv::Mat &input, cv::Mat &output); 
	void applyBirdEye(cv::Mat &input, cv::Mat &output); 
	void edgeDetect(cv::Mat &input, cv::Mat &output_thresh, cv::Mat &output_edges, int &threshold_value, int morph_size, int morph_elem);
	void applyMask(cv::Mat &input, cv::Mat &mask, cv::Mat &output);
    void detectLines(cv::Mat &input, int &P_min_votes, int &P_max_gap, int &P_min_len);

	//
	void draw_data(cv::Mat &output, cv::Point points[]);

	//void drawLines(cv::Mat &output);

	//	
	void calculate_all_slopes();
	void sort_lines(bool l_change);

	float average_slope(std::vector<Punkt> &punkty);
 	int average_pos(std::vector<Punkt> &punkty);

	//	
	void save_for_next_step();
	void draw_middle_points(cv::Mat &output);

	//
	void send_data_to_main(int &detected_middle_pos_near, int &left_lane_angle_st, int &right_lane_angle_st, char &flags_to_UART);
	void restart_lane_detection(int &UART_offset, bool &check_for_offset, int &mask_y, int &mask_widthint, int &mask_y_1, int &mask_y_2, int &mask_x_l, int &mask_x_r);
	
	//
	void cancel_offset(int &UART_offset, bool &check_for_offset);
	void change_lane(int &UART_offset, bool &check_for_offset);
	void display_last_middle();

	//
	void horizontal_line (cv::Mat frame);
	//void dynamic_mask (int &left_x, int &right_x, int mask_y, int mask_width);
	void dynamic_mask (int &left_x, int &right_x, int &mask_y, int &mask_width);
};
