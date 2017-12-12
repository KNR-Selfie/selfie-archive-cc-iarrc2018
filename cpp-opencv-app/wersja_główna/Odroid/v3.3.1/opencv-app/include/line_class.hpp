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

public:
	LineDetector()
	{
		last_top_middle_point.coordinates = cv::Point(320,150);
		last_bottom_middle_point.coordinates = cv::Point(320,250);
		
		last_top_middle_point.slope = 999;
		last_bottom_middle_point.slope = 999;
	}

	//Dane do wysylki
	int srodek = 320;
	float lewy_kat = 90;
	float prawy_kat = 90;

	//
    void applyBlur(cv::Mat &input, cv::Mat &output);  
	void edgeDetect(cv::Mat &input, cv::Mat &output_thresh, cv::Mat &output_edges, int &threshold_value);
	void applyMask(cv::Mat &input, cv::Mat &mask, cv::Mat &output);
    void detectLines(cv::Mat &input);
	void drawLines(cv::Mat &output);

	//	
	void calculate_all_slopes();
	void sort_lines();
	float average_slope(std::vector<Punkt> &punkty);
 	int average_pos(std::vector<Punkt> &punkty);

	//	
	void save_for_next_step(float &new_left_slope, float &new_right_slope, int &new_center_pos);
};
